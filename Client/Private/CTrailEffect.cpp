#include "CTrailEffect.h"
#include "CGameInstance.h"
#include "VertexData.h"
#include "CShader.h"
#include "CTexture.h"


USING(Client)
CTrailEffect::CTrailEffect(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CGameObject(pDevice,pContext)
{
}

CTrailEffect::CTrailEffect(const CTrailEffect& rhs)
    : CGameObject(rhs)
{
}

HRESULT CTrailEffect::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CTrailEffect::Initialize_Copytype(void* pArg)
{

    TrailDesc Desc;
    Desc.ObjTag = L"Trail";

    CTransform::TRANSFORM_DESC TransDesc;
    Desc.TransformDesc = &TransDesc;

    if (FAILED(__super::Initialize_Copytype(&Desc)))
        return E_FAIL;
   
	m_fTopOffSet = Desc.TopOffset;
	m_fBottomOffSet = Desc.BottomOffSet;
	m_fWidth = Desc.fWidth;
	m_fLifeTime = Desc.fLifeTime;


    m_iVertexStride = sizeof(VTXPOSTEX);

    if (FAILED(Ready_Component()))
        return E_FAIL;




    m_pShader = m_pGameInstance->Find_Shader(L"Default");
    m_pTexture = m_pGameInstance->Find_Texture(L"Default");
    return S_OK;
}

void CTrailEffect::Update_Priority(_float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
}

void CTrailEffect::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
    CheckNull(m_pWeaponMatrix);
    CheckNull(m_ParentMatrix);

    _matrix matWeapon = XMLoadFloat4x4(m_pWeaponMatrix);
    _matrix ParentMatrix = XMLoadFloat4x4(m_ParentMatrix);
    //칼 위치에서의 위아래 좌표계산

    _matrix matWeaponWolrd = XMMatrixMultiply(matWeapon, ParentMatrix);

    _vector vTopPos = XMVector3TransformCoord(XMVectorSet(0.f, 0.f,-1.5f, 1.f), matWeaponWolrd);
    _vector vBottomPos = XMVector3TransformCoord(XMVectorSet(0.f, 0.f, 0.f, 1.f), matWeaponWolrd);

    TrailNode NewNode;
    XMStoreFloat3(&NewNode.vTop, vTopPos);
    XMStoreFloat3(&NewNode.vBottom, vBottomPos);
    NewNode.m_fTime = 0.0f;

    m_TrailList.push_front(NewNode);

    for (auto iter = m_TrailList.begin(); iter != m_TrailList.end(); )
    {
        iter->m_fTime += fTimeDelta;

        // 수명이 다했거나, 너무 길어지면 뒤에서부터 자름
        if (iter->m_fTime > m_fLifeTime || m_TrailList.size() > m_iMaxRect)
        {
            iter = m_TrailList.erase(iter); // 삭제
        }
        else
        {
            ++iter;
        }
    }

}

void CTrailEffect::Update_Late(_float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);

}

void CTrailEffect::Update_Render(_float fTimeDelta)
{
    __super::Update_Render(fTimeDelta);
    m_pGameInstance->Add_RenderObject(ENUM_TO_UINT(RENDERGROUP::NONALPHA), this);

}

HRESULT CTrailEffect::Render()
{
    // 점이 2개 미만이면 선을 못 그으니 패스
    if (m_TrailList.size() < 2) return S_OK;

    // 1. 정점 데이터 생성
    vector<VTXPOSTEX> Vertices;
    Vertices.reserve(m_TrailList.size() * 2);

    _uint iIndex = 0;
    _uint iSize = m_TrailList.size();

    for (auto& Node : m_TrailList)
    {
        float fU = (float)iIndex / (float)(iSize - 1);

        // 위쪽 점
        VTXPOSTEX vTop;
        vTop.vPosition = Node.vTop;
        vTop.vTexcoord = _float2(fU, 0.0f);
        Vertices.push_back(vTop);

        // 아래쪽 점 (UV 수정됨!)
        VTXPOSTEX vBottom;
        vBottom.vPosition = Node.vBottom;
        vBottom.vTexcoord = _float2(fU, 1.0f); // ★ U는 같고 V는 1
        Vertices.push_back(vBottom);

        iIndex++;
    }

    // 2. GPU 전송 (Map/Unmap)
    D3D11_MAPPED_SUBRESOURCE tMappedSubResource;
    m_pContext->Map(m_pVB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &tMappedSubResource);
    memcpy(tMappedSubResource.pData, Vertices.data(), sizeof(VTXPOSTEX) * Vertices.size());
    m_pContext->Unmap(m_pVB.Get(), 0);


    // 3. 셰이더 설정 (가장 중요!)

    // ★ [수정 1] 월드 행렬은 Identity로!
    _float4x4 matIdentity;
    XMStoreFloat4x4(&matIdentity, XMMatrixIdentity());
    m_pShader->Bind_Matrix("g_WorldMatrix", matIdentity);

  
    // 텍스처 바인딩
    if (FAILED(m_pTexture->Bind_ShaderResource(m_pShader, "texture0", 0)))
        return E_FAIL;

    if (FAILED(m_pShader->Bind_Float("g_Alpha",1.f)))
        return E_FAIL;

    m_pShader->Begin("Test"); // 0번 패스 실행


    // 4. 파이프라인 설정
    _uint iStride = sizeof(VTXPOSTEX);
    _uint iOffset = 0;

    m_pContext->IASetVertexBuffers(0, 1, m_pVB.GetAddressOf(), &iStride, &iOffset);
    m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);



    // 5. 그리기
    m_pContext->Draw(Vertices.size(), 0);

    // 복구
    m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


    return S_OK;
}

CTrailEffect* CTrailEffect::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext, void* pArg)
{
    CTrailEffect* pInstance = new CTrailEffect(_pDevice, _pDeviceContext);

    if (FAILED(pInstance->Initialize_Copytype(pArg)))
    {
        MSG_BOX("Failed to Created : CTrailEffect");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CTrailEffect::Clone(void* pArg)
{
    CTrailEffect* pInstance = new CTrailEffect(*this);

    if (FAILED(pInstance->Initialize_Copytype(pArg)))
    {
        MSG_BOX("Failed to Created : CTrailEffect");
        Safe_Release(pInstance);
    }
    return pInstance;
}


HRESULT CTrailEffect::Ready_Component()
{
    // [버퍼 생성]
    D3D11_BUFFER_DESC tBufferDesc;
    ZeroMemory(&tBufferDesc, sizeof(D3D11_BUFFER_DESC));



    // 넉넉하게 잡으세요. 예: 100개 세그먼트면 200개 정점.
    tBufferDesc.ByteWidth = sizeof(VTXPOSTEX) * m_iMaxRect * 2;


    tBufferDesc.Usage = D3D11_USAGE_DYNAMIC;       // GPU가 읽고, CPU가 쓴다
    tBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;  // 정점 버퍼다
    tBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // CPU가 'Map'으로 접근하겠다
    tBufferDesc.MiscFlags = 0;
    tBufferDesc.StructureByteStride = sizeof(VTXPOSTEX);

    // 3. 초기 데이터 없이 생성 (빈 그릇만 만듦)
    // pInitialData에 nullptr을 넣습니다. 어차피 Update에서 채울 거니까요.
    if (FAILED(m_pDevice->CreateBuffer(&tBufferDesc, nullptr, &m_pVB)))
    {
        return E_FAIL;
    }
}

void CTrailEffect::Start_Trail(const _float4x4* pWeaponWorldMatrix, const _float4x4* ParentMatrix)
{
    m_pWeaponMatrix = pWeaponWorldMatrix;
    m_ParentMatrix = ParentMatrix;

}

void CTrailEffect::Stop_Trail()
{
}

void CTrailEffect::Free()
{
    __super::Free();
}


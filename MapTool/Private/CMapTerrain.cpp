#include "CMapTerrain.h"
#include "CGameInstance.h"

#include "CVIBuffer_CustomTerrain.h"
#include "CShader.h"
#include "CTexture.h"
#include "Client_Defines.h"


USING(MapTool)
CMapTerrain::CMapTerrain(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CTerrain_Base(pDevice,pContext)
{
}

CMapTerrain::CMapTerrain(const CMapTerrain& Prototype)
    : CTerrain_Base(Prototype)
{
}

HRESULT CMapTerrain::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;


    return S_OK;
}

HRESULT CMapTerrain::Initialize_Copytype(void* pArg)
{
    if (FAILED(__super::Initialize_Copytype(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components(pArg)))
        return E_FAIL;

    if (FAILED(CreateRasterizerState()))
        return E_FAIL;

    
    return S_OK;
}

void CMapTerrain::Update_Priority(_float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
}

void CMapTerrain::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CMapTerrain::Update_Late(_float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);
}

void CMapTerrain::Update_Render(_float fTimeDelta)
{
    __super::Update_Render(fTimeDelta);

}

HRESULT CMapTerrain::Render()
{
    // 기존 상태 저장
    ComPtr<ID3D11RasterizerState> pOldRS = nullptr;
    m_pContext->RSGetState(pOldRS.GetAddressOf());

    // 와이어프레임 상태로 설정
    m_pContext->RSSetState(m_pWireframeRS.Get());

    if (FAILED(__super::Render()))
        return E_FAIL;
   

    m_pContext->RSSetState(pOldRS.Get());

    return S_OK;
}

HRESULT CMapTerrain::CreateRasterizerState()
{

    D3D11_RASTERIZER_DESC desc{};
    desc.FillMode = D3D11_FILL_WIREFRAME;  // 선 모드
    desc.CullMode = D3D11_CULL_NONE;       // 뒷면도 그리게
    desc.FrontCounterClockwise = FALSE;
    desc.DepthClipEnable = TRUE;           // 보통 TRUE

    if (FAILED(m_pDevice->CreateRasterizerState(&desc, m_pWireframeRS.GetAddressOf())))
    {
        MSG_BOX("Failed to Create Wireframe RasterizerState");
        return E_FAIL;
    }

}

void CMapTerrain::Update_Terrain(_float NumX, _float NumZ)
{
    CheckNull(m_pCustomBuffer);
    if(m_pCustomBuffer)
        m_pCustomBuffer->ResizeBuffer(NumX, NumZ);
}

HRESULT CMapTerrain::Ready_Components(void* pArg)
{

    CComponent* pBuffer_Terrain = dynamic_cast<CVIBuffer_CustomTerrain*>(m_pGameInstance->Clone_Prototype
    (PROTOTYPE::COMPONENT, 0, PROTO_COMPONENT_NAME(L"VIBuffer_CustomTerrain"), pArg));


    if (FAILED(__super::Add_Component(COMPONENT_TYPE::VIBUFFER_TERRAIN, pBuffer_Terrain, (CComponent**)&m_pVIBufferCom)))
        return E_FAIL;

    m_pCustomBuffer = dynamic_cast<CVIBuffer_CustomTerrain*>(pBuffer_Terrain);
    if (m_pCustomBuffer)
        Safe_AddRef(m_pCustomBuffer);


    return S_OK;
}


CMapTerrain* CMapTerrain::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
    CMapTerrain* pInstance = new CMapTerrain(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create :CMapTerrain ");
        Safe_Release(pInstance);

    }



    return pInstance;
}

CGameObject* CMapTerrain::Clone(void* pArg)
{
    CMapTerrain* pInstance = new CMapTerrain(*this);
    if (FAILED(pInstance->Initialize_Copytype(pArg)))
    {
        MSG_BOX("Failed to Cloned :CMapTerrain ");
        Safe_Release(pInstance);

    }
    return pInstance;
}

void CMapTerrain::Free()
{
    __super::Free();

    Safe_Release(m_pCustomBuffer);
}

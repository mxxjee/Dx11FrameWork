#include "CMapGrid.h"
#include "CGameInstance.h"

#include "CVIBuffer_CustomTerrain.h"
#include "CShader.h"
#include "CTexture.h"
#include "Client_Defines.h"

#include "CTerrain_Highlight.h"


USING(MapTool)
CMapGrid::CMapGrid(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CTerrain_Base(pDevice,pContext)
{
}

CMapGrid::CMapGrid(const CMapGrid& Prototype)
    : CTerrain_Base(Prototype)
{
}

HRESULT CMapGrid::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;


    return S_OK;
}

HRESULT CMapGrid::Initialize_Copytype(void* pArg)
{
    if (FAILED(__super::Initialize_Copytype(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components(pArg)))
        return E_FAIL;

    if (FAILED(CreateRasterizerState()))
        return E_FAIL;

    
    return S_OK;
}

void CMapGrid::Update_Priority(_float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
}

void CMapGrid::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CMapGrid::Update_Late(_float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);
}

void CMapGrid::Update_Render(_float fTimeDelta)
{
    __super::Update_Render(fTimeDelta);

}

HRESULT CMapGrid::Render()
{
    // 기존 상태 저장
    ComPtr<ID3D11RasterizerState> pOldRS = nullptr;
    m_pContext->RSGetState(pOldRS.GetAddressOf());

    // 와이어프레임 상태로 설정
    m_pContext->RSSetState(m_pWireframeRS.Get());
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    if (FAILED(m_pShader->Begin(m_passName)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Resource()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

   

    m_pContext->RSSetState(pOldRS.Get());

    return S_OK;
}

HRESULT CMapGrid::CreateRasterizerState()
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

    return S_OK;
}

void CMapGrid::Update_Terrain(_uint NumX, _uint NumZ)
{
    CheckNull(m_pCustomBuffer);
    if(m_pCustomBuffer)
        m_pCustomBuffer->ResizeBuffer(NumX, NumZ);
}
HRESULT CMapGrid::Ready_Components(void* pArg)
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

HRESULT CMapGrid::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShader, "g_WorldMatrix")))
        return E_FAIL;

    return S_OK;
}


CMapGrid* CMapGrid::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
    CMapGrid* pInstance = new CMapGrid(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create :CMapGrid ");
        Safe_Release(pInstance);

    }



    return pInstance;
}

CGameObject* CMapGrid::Clone(void* pArg)
{
    CMapGrid* pInstance = new CMapGrid(*this);
    if (FAILED(pInstance->Initialize_Copytype(pArg)))
    {
        MSG_BOX("Failed to Cloned :CMapGrid ");
        Safe_Release(pInstance);

    }
    return pInstance;
}

void CMapGrid::Free()
{
    __super::Free();


    Safe_Release(m_pCustomBuffer);
}

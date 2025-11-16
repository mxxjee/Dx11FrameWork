#include "CMapToolCell.h"
#include "CVIBuffer_Triangle.h"
#include "CGameInstance.h"
#include "CShader.h"



CMapToolCell::CMapToolCell(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :m_pDevice(pDevice), m_pContext(pContext), m_pGameInstance(CGameInstance::GetInstance())
{
    Safe_AddRef(m_pGameInstance);
}

void CMapToolCell::Set_PreviewPoints(const deque<PreviewPoint>& New)
{
    for (int i = 0; i < New.size(); ++i)
    {
        m_PreviewPoints[i] = New[i];
    }
}

HRESULT CMapToolCell::Initialize_Prototype(void* pArg)
{
    if (FAILED(Ready_Components(pArg)))
        return E_FAIL;


    m_pShader = m_pGameInstance->Find_Shader(L"VtxPosCor");
    Safe_AddRef(m_pShader);

    XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());

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

HRESULT CMapToolCell::Ready_Components(void* pArg)
{
    CheckNullResult(pArg, E_FAIL);
    MAPTOOLCELL_DESC* pDesc = static_cast<MAPTOOLCELL_DESC*>(pArg);

    CComponent* pBuffer = dynamic_cast<CComponent*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::COMPONENT, 0, PROTO_COMPONENT_NAME(L"VIBuffer_Triangle"), pDesc->TriangleCom));
    m_pVIBufferCom = dynamic_cast<CVIBuffer_Triangle*>(pBuffer);
    if (pDesc->TriangleCom)
    {
        CVIBuffer_Triangle::TRIANGLEBUFFER_DESC* pTriangleDesc = static_cast<CVIBuffer_Triangle::TRIANGLEBUFFER_DESC*>(pDesc->TriangleCom);
        
        m_vPoints[ENUM_TO_UINT(POINTType::A)] = pTriangleDesc->v0;
        m_vPoints[ENUM_TO_UINT(POINTType::B)] = pTriangleDesc->v1;
        m_vPoints[ENUM_TO_UINT(POINTType::C)] = pTriangleDesc->v2;


    }


    return S_OK;
}

HRESULT CMapToolCell::Render()
{
    CheckNullResult(m_pShader, E_FAIL);

    ComPtr<ID3D11RasterizerState> pOldRS = nullptr;
    m_pContext->RSGetState(pOldRS.GetAddressOf());

    m_pContext->RSSetState(m_pWireframeRS.Get());

    if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", WorldMatrix)))
        return E_FAIL;

    _float4x4 ViewProjMatrix;
    XMStoreFloat4x4(&ViewProjMatrix, m_pGameInstance->Get_ViewProjMatrix(ENUM_TO_UINT(CAMERA_TYPE::FREE)));

    if (m_pShader->Bind_Matrix("g_ViewProjMatrix", ViewProjMatrix))
        return E_FAIL;

    if (m_pShader->Bind_Vector("g_Color", _float4(0.f, 1.f, 0.f, 1.f)))
        return E_FAIL;

    if (FAILED(m_pShader->Begin("Default")))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Resource()))
        return E_FAIL;          //IA단계

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    m_pContext->RSSetState(pOldRS.Get());


    return S_OK;
}

CMapToolCell* CMapToolCell::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg)
{
    CMapToolCell* pInstnace = new CMapToolCell(pDevice, pContext);
    if (FAILED(pInstnace->Initialize_Prototype(pArg)))
    {
        MSG_BOX("FAiled To Create:CMAptool");
        Safe_Release(pInstnace);

    }
    return pInstnace;
}

void CMapToolCell::Free()
{
    __super::Free();

    Safe_Release(m_pShader);
    Safe_Release(m_pGameInstance);
    Safe_Release(m_pVIBufferCom);

}

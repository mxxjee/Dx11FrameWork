#include "CNavEditPreview.h"
#include "CComponent.h"
#include "CGameInstance.h"
#include "CVIBuffer_Triangle.h"
#include "CShader.h"

CNavEditPreview::CNavEditPreview(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :m_pDevice(pDevice), m_pContext(pContext), m_pGameInstance(CGameInstance ::GetInstance())
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CNavEditPreview::Initialize()
{
    if (FAILED(Ready_Components()))
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

void CNavEditPreview::UpdatePoints(deque<PreviewPoint> Points)
{
    _float3 fPoints[3] = {
        _float3(0.f,0.f,0.f),_float3(0.f,0.f,0.f),_float3(0.f,0.f,0.f) };

    int iIdx = 0;

    while (!Points.empty())
    {
        _float3 vPos = Points.front().vPos;
        fPoints[iIdx] = vPos;

        Points.pop_front();

        
        if(!XMVector3Equal(XMLoadFloat3(&vPos), XMVectorSet(-999.f, -999.f, -999.f, 1.f)))
            ++iIdx;
    }

        

    if (iIdx == 1)
        m_pVIBufferCom->Set_PrimitiveType(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

    else if (iIdx == 2)
        m_pVIBufferCom->Set_PrimitiveType(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

    else if(iIdx ==3)
        m_pVIBufferCom->Set_PrimitiveType(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    else
        m_pVIBufferCom->Set_PrimitiveType(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    m_pVIBufferCom->UpdatePoints(fPoints[0], fPoints[1], fPoints[2]);

}

HRESULT CNavEditPreview::Render()
{
    CheckNullResult(m_pShader,E_FAIL);

    ComPtr<ID3D11RasterizerState> pOldRS = nullptr;
    m_pContext->RSGetState(pOldRS.GetAddressOf());

    m_pContext->RSSetState(m_pWireframeRS.Get());

    if(FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", WorldMatrix)))
        return E_FAIL;

    _float4x4 ViewProjMatrix;
    XMStoreFloat4x4(&ViewProjMatrix, m_pGameInstance->Get_ViewProjMatrix(ENUM_TO_UINT(CAMERA_TYPE::FREE)));

    if (m_pShader->Bind_Matrix("g_ViewProjMatrix", ViewProjMatrix))
        return E_FAIL;

    if (m_pShader->Bind_Vector("g_Color", _float4(1.f, 0.f, 0.f, 1.f)))
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

HRESULT CNavEditPreview::Ready_Components()
{
  
    CComponent* pBuffer = dynamic_cast<CComponent*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::COMPONENT, 0, PROTO_COMPONENT_NAME(L"VIBuffer_Triangle"), nullptr));
    m_pVIBufferCom = dynamic_cast<CVIBuffer_Triangle*>(pBuffer);

    return S_OK;
    
}

void CNavEditPreview::Set_Shader(CShader* pShader)
{
    m_pShader = pShader; Safe_AddRef(pShader);
}

CNavEditPreview* CNavEditPreview::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
    CNavEditPreview* pInstance = new CNavEditPreview(pDevice, pContext);
    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Create :CQuad ");
        Safe_Release(pInstance);

    }
    return pInstance;

}

void CNavEditPreview::Free()
{
    __super::Free();

    Safe_Release(m_pShader);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pGameInstance);

}

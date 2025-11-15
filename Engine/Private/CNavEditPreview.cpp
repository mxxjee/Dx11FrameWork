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

    m_pShader = m_pGameInstance->Find_Shader(L"Default");
    Safe_AddRef(m_pShader);

    return S_OK;
}

void CNavEditPreview::UpdatePoints(_float3 p0, _float3 p1, _float3 p2)
{
   
    m_pVIBufferCom->UpdatePoints(p0, p1, p2);


}

HRESULT CNavEditPreview::Render()
{
    if (FAILED(m_pShader->Begin("Default")))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Resource()))
        return E_FAIL;          //IA´Ü°è

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CNavEditPreview::Ready_Components()
{
  
    CComponent* pBuffer = dynamic_cast<CComponent*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::COMPONENT, 0, PROTO_COMPONENT_NAME(L"VIBuffer_Triangle"), nullptr));
    m_pVIBufferCom = dynamic_cast<CVIBuffer_Triangle*>(pBuffer);

    return S_OK;
    
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
    Safe_Release(m_pGameInstance);

}

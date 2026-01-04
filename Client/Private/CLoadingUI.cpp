#include "CLoadingUI.h"
#include "CShader.h"
#include "CGameInstance.h"


USING(Client)
CLoadingUI::CLoadingUI(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
    :CPanel(_pDevice,_pDeviceContext)
{
}

CLoadingUI::CLoadingUI(const CLoadingUI& rhs)
    : CPanel(rhs)
{
}

HRESULT CLoadingUI::Initialize_Prototype()
{
   
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;


    return S_OK;
}

HRESULT CLoadingUI::Initialize_Copytype(void* pArg)
{
    CUI* pUIDesc = static_cast<CUI*>(pArg);

    if (FAILED(__super::Initialize_Copytype(pArg)))
        return E_FAIL;


    m_pUICom->PlayAnim(UIAnimType::ALPHA);
    m_pUICom->PlayAnim(UIAnimType::ROTATION);

    m_passName = "Loading";

    return S_OK;
}

void CLoadingUI::Update_Priority(_float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
}

void CLoadingUI::Update(_float fTimeDelta)
{

    __super::Update(fTimeDelta);
}

void CLoadingUI::Update_Late(_float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);
}

void CLoadingUI::Update_Render(_float fTimeDelta)
{
    __super::Update_Render(fTimeDelta);
}

HRESULT CLoadingUI::Render()
{

   
    m_pGameInstance->CopyData_Buffer("LoadingBuffer", &m_fProgress, sizeof(_float4));


    __super::Render();

    return S_OK;
}

CPanel* CLoadingUI::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
{

    CLoadingUI* pInstance = new CLoadingUI(_pDevice, _pDeviceContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create :CLoadingUI ");
        Safe_Release(pInstance);

    }

    return pInstance;
}

CGameObject* CLoadingUI::Clone(void* pArg)
{
    CLoadingUI* pInstance = new CLoadingUI(*this);
    if (FAILED(pInstance->Initialize_Copytype(pArg)))
    {
        MSG_BOX("Failed to Cloned :CLoadingUI ");
        Safe_Release(pInstance);

    }
    return pInstance;
}

void CLoadingUI::Free()
{
    __super::Free();
}

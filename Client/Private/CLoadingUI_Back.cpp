#include "CLoadingUI_Back.h"
#include "CShader.h"

USING(Client)
CLoadingUI_Back::CLoadingUI_Back(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
    :CPanel(_pDevice,_pDeviceContext)
{
}

CLoadingUI_Back::CLoadingUI_Back(const CLoadingUI_Back& rhs)
    : CPanel(rhs)
{
}

HRESULT CLoadingUI_Back::Initialize_Prototype()
{
   
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;


    return S_OK;
}

HRESULT CLoadingUI_Back::Initialize_Copytype(void* pArg)
{
    CUI* pUIDesc = static_cast<CUI*>(pArg);

    if (FAILED(__super::Initialize_Copytype(pArg)))
        return E_FAIL;


    m_passName = "BlackAndWhite";


    return S_OK;
}

void CLoadingUI_Back::Update_Priority(_float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
}

void CLoadingUI_Back::Update(_float fTimeDelta)
{

    __super::Update(fTimeDelta);
}

void CLoadingUI_Back::Update_Late(_float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);
}

void CLoadingUI_Back::Update_Render(_float fTimeDelta)
{
    __super::Update_Render(fTimeDelta);
}

HRESULT CLoadingUI_Back::Render()
{
    __super::Render();
    return S_OK;
}

CPanel* CLoadingUI_Back::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
{

    CLoadingUI_Back* pInstance = new CLoadingUI_Back(_pDevice, _pDeviceContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create :CLoadingUI_Back ");
        Safe_Release(pInstance);

    }

    return pInstance;
}

CGameObject* CLoadingUI_Back::Clone(void* pArg)
{
    CLoadingUI_Back* pInstance = new CLoadingUI_Back(*this);
    if (FAILED(pInstance->Initialize_Copytype(pArg)))
    {
        MSG_BOX("Failed to Cloned :CLoadingUI_Back ");
        Safe_Release(pInstance);

    }
    return pInstance;
}

void CLoadingUI_Back::Free()
{
    __super::Free();
}

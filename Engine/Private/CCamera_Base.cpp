#include "CCamera_Base.h"
#include "CGameInstance.h"

CCamera_Base::CCamera_Base(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CGameObject(pDevice,pContext)
{
}

CCamera_Base::CCamera_Base(const CCamera_Base& rhs)
    :CGameObject(rhs)
{
}

HRESULT CCamera_Base::Initialize_Prototype()
{
    
    return S_OK;
}

HRESULT CCamera_Base::Initialize_Copytype(void* pArg)
{
    if(FAILED(__super::Initialize_Copytype(pArg)))
        return E_FAIL;

    m_pMainShader = m_pGameInstance->Find_Shader(L"Default");
    return S_OK;
}

void CCamera_Base::Free()
{
    __super::Free();
}



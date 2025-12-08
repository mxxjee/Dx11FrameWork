#include "CInteraction_Rock.h"
#include "CCollider_Base.h"

USING(Client)
CInteraction_Rock::CInteraction_Rock(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CInteractionObject(pDevice,pContext)
{
}

CInteraction_Rock::CInteraction_Rock(const CInteraction_Rock& rhs)
    : CInteractionObject(rhs)
{
}

HRESULT CInteraction_Rock::Initialize_Copytype(void* pArg)
{
    if(FAILED(__super::Initialize_Copytype(pArg)))
        return E_FAIL;

    m_pCollider->Set_Trigger(false);

    return S_OK;
}

CInteraction_Rock* CInteraction_Rock::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
{
    CInteraction_Rock* pInstance = new CInteraction_Rock(_pDevice, _pDeviceContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create :CInteraction_Rock ");
        Safe_Release(pInstance);

    }



    return pInstance;
}

CGameObject* CInteraction_Rock::Clone(void* pArg)
{
    CInteraction_Rock* pInstance = new CInteraction_Rock(*this);
    if (FAILED(pInstance->Initialize_Copytype(pArg)))
    {
        MSG_BOX("Failed to Clone :CInteraction_Rock ");
        Safe_Release(pInstance);

    }



    return pInstance;
}

void CInteraction_Rock::Free()
{
    __super::Free();
}

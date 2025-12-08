#include "CInteraction_Lawn.h"

USING(Client)
CInteraction_Lawn::CInteraction_Lawn(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CInteractionObject(pDevice, pContext)
{
}

CInteraction_Lawn::CInteraction_Lawn(const CInteraction_Lawn& rhs)
    : CInteractionObject(rhs)
{
}

CInteraction_Lawn* CInteraction_Lawn::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
{
    CInteraction_Lawn* pInstance = new CInteraction_Lawn(_pDevice, _pDeviceContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create :CInteraction_Lawn ");
        Safe_Release(pInstance);

    }



    return pInstance;
}

CGameObject* CInteraction_Lawn::Clone(void* pArg)
{
    CInteraction_Lawn* pInstance = new CInteraction_Lawn(*this);
    if (FAILED(pInstance->Initialize_Copytype(pArg)))
    {
        MSG_BOX("Failed to Clone :CInteraction_Lawn ");
        Safe_Release(pInstance);

    }



    return pInstance;
}

void CInteraction_Lawn::Free()
{
    __super::Free();
}

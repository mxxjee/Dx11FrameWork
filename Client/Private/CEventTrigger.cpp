#include "CEventTrigger.h"
#include "CBoxColliderComponent.h"
#include "Client_Defines.h"


USING(Client)
CEventTrigger::CEventTrigger(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CTrigger_Box(pDevice,pContext)
{
}

CEventTrigger::CEventTrigger(const CEventTrigger& rhs)
    :CTrigger_Box(rhs)
{
}

HRESULT CEventTrigger::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CEventTrigger::Initialize_Copytype(void* pArg)
{
    if (FAILED(__super::Initialize_Copytype(pArg)))
        return E_FAIL;

    EventTriggerDesc* pDesc = static_cast<EventTriggerDesc*>(pArg);
   
    CheckNullResult(pBoxCollider, S_OK);
    pBoxCollider->Set_ColGroup(ENUM_TO_UINT(COLLISION_GROUP::TRIGGER));
    pBoxCollider->Set_Owner(this);
    
    m_EnterFunction = pDesc->EnterFunc;
    m_StayFunction = pDesc->StayFunc;
    m_EndFunction = pDesc->EnterFunc;

    return S_OK;
}

CEventTrigger* CEventTrigger::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContex)
{
    CEventTrigger* pInstance = new CEventTrigger(_pDevice, _pDeviceContex);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create :CEventTrigger ");
        Safe_Release(pInstance);

    }



    return pInstance;
}

CGameObject* CEventTrigger::Clone(void* pArg)
{
    CEventTrigger* pInstance = new CEventTrigger(*this);
    if (FAILED(pInstance->Initialize_Copytype(pArg)))
    {
        MSG_BOX("Failed to Cloned :CEventTrigger ");
        Safe_Release(pInstance);

    }
    return pInstance;
}

void CEventTrigger::Free()
{
    __super::Free();
}

void CEventTrigger::OnCollisionEnter(_uint iGroup, CCollider_Base* pOther)
{
    CGameObject* pOwner = pOther->Get_Owner();

    CheckNull(pOwner);
    switch (COLLISION_GROUP(iGroup))
    {

    case Client::COLLISION_GROUP::PLAYER:
        if (m_EnterFunction)
            m_EnterFunction();
        break;

    }
}

void CEventTrigger::OnCollisionStay(_uint iGroup, CCollider_Base* pOther)
{
    CGameObject* pOwner = pOther->Get_Owner();

    CheckNull(pOwner);
    switch (COLLISION_GROUP(iGroup))
    {

    case Client::COLLISION_GROUP::PLAYER:
        if (m_StayFunction)
            m_StayFunction();
        break;

    }
}

void CEventTrigger::OnCollisionExit(_uint iGroup, CCollider_Base* pOther)
{
    CGameObject* pOwner = pOther->Get_Owner();

    CheckNull(pOwner);
    switch (COLLISION_GROUP(iGroup))
    {

    case Client::COLLISION_GROUP::PLAYER:
        if (m_EndFunction)
            m_EndFunction();
        break;

    }
}

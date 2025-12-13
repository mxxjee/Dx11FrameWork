#include "CRoomTrigger.h"
#include "CCollider_Base.h"
#include "Client_Defines.h"
#include "CRoom_Manager.h"
USING(Client)
CRoomTrigger::CRoomTrigger(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CTrigger_Box(pDevice,pContext)
{
}

CRoomTrigger::CRoomTrigger(const CRoomTrigger& rhs)
    :CTrigger_Box(rhs)
{
}

HRESULT CRoomTrigger::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CRoomTrigger::Initialize_Copytype(void* pArg)
{
    if (FAILED(__super::Initialize_Copytype(pArg)))
        return E_FAIL;

    RoomTriggerDesc* pDesc = static_cast<RoomTriggerDesc*>(pArg);
    m_NextRoomKey = pDesc->m_nextKey;

    return S_OK;
}

void CRoomTrigger::OnCollisionEnter(_uint iGroup, CCollider_Base* pOther)
{

    CGameObject* pOwner = pOther->Get_Owner();

    CheckNull(pOwner);
    switch (COLLISION_GROUP(iGroup))
    {
  
    case Client::COLLISION_GROUP::PLAYER:
        if (m_NextRoomKey == "Level_Town")
            m_pGameInstance->Pop_Level();
      

        else
            CRoom_Manager::GetInstance()->Switch_Room(m_NextRoomKey);
        break;

    }
}

void CRoomTrigger::OnCollisionStay(_uint iGroup, CCollider_Base* pOther)
{
}

void CRoomTrigger::OnCollisionExit(_uint iGroup, CCollider_Base* pOther)
{
}

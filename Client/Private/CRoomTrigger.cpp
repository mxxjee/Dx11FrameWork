#include "CRoomTrigger.h"
#include "CCollider_Base.h"
#include "Client_Defines.h"
#include "CRoom_Manager.h"
#include "CBoxColliderComponent.h"
#include "CGameManager.h"
#include "CPlayer.h"
#include "CNavMesh_Manager.h"
#include "CFadeScreen.h"
#include "CGameManager.h"

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


    CheckNullResult (pBoxCollider,S_OK);
    pBoxCollider->Set_ColGroup(ENUM_TO_UINT(COLLISION_GROUP::TRIGGER));
    pBoxCollider->Set_Owner(this);


    UIGroup* pGroup = m_pGameInstance->Get_UIGroup(L"FadeScreenGroup");
    m_pFadeScreen = dynamic_cast<CFadeScreen*>(pGroup->Find(L"FadeScreen"));


    return S_OK;
}

CRoomTrigger* CRoomTrigger::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContex)
{
    CRoomTrigger* pInstance = new CRoomTrigger(_pDevice, _pDeviceContex);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create :CRoomTrigger ");
        Safe_Release(pInstance);

    }



    return pInstance;
}

CGameObject* CRoomTrigger::Clone(void* pArg)
{
    CRoomTrigger* pInstance = new CRoomTrigger(*this);
    if (FAILED(pInstance->Initialize_Copytype(pArg)))
    {
        MSG_BOX("Failed to Cloned :CRoomTrigger ");
        Safe_Release(pInstance);

    }
    return pInstance;
}

void CRoomTrigger::Free()
{
    __super::Free();
}

void CRoomTrigger::OnCollisionEnter(_uint iGroup, CCollider_Base* pOther)
{
    CheckTrue(m_pGameInstance->Get_IsLoading());

    CGameObject* pOwner = pOther->Get_Owner();

    CheckNull(pOwner);
    switch (COLLISION_GROUP(iGroup))
    {
  
    case Client::COLLISION_GROUP::PLAYER:
        if (m_NextRoomKey == "Level_Town")
        {
            m_pGameInstance->StopSoundFade(SOUND_BGM, 1.f);
            m_pGameInstance->StopSoundFade(SOUND_AMBIENCE, 0.5f);

            m_pFadeScreen->PlayFadeIn();

            m_pGameInstance->Invoke(2.f, 0.f, false, false, [m_pGameInstance= m_pGameInstance]()
                {
                    m_pGameInstance->Pop_Level();
                }, CGameManager::GetInstance()->Get_MainPlayer());
    
            
           // m_pGameInstance->Set_IsLoading(true);
        }

        else if (m_NextRoomKey == "Level_Dungeon")
        {
            m_pGameInstance->StopSoundFade(SOUND_BGM, 1.f);
            m_pGameInstance->StopSoundFade(SOUND_AMBIENCE, 0.5f);
            m_pFadeScreen->Set_FadeInEndFunc([this]()
                {
                    LevelArgs args;
                    args.iNextLevelID = ENUM_TO_UINT(LEVEL_ID::DUNGEON);
                    args.changeType = LEVELCHANGETYPE::PUSH;
                    args.loadingChangeType = LEVELCHANGETYPE::PUSH;
                    args.m_iLevelID = ENUM_TO_UINT(LEVEL_ID::LOADING);




                    if (FAILED(m_pGameInstance->Level_Changer(
                        ENUM_TO_UINT(LEVEL_ID::LOADING),
                        args)))
                        return;
                });

            m_pFadeScreen->PlayFadeIn();

        }

        else
        {
            m_pGameInstance->StopSoundFade(SOUND_BGM, 1.f);
            m_pGameInstance->StopSoundFade(SOUND_AMBIENCE, 0.5f);

             CRoom_Manager::GetInstance()->Request_Room(m_NextRoomKey);

             CGameManager* pGameManager = CGameManager::GetInstance();

             CPlayer* pPlayer = pGameManager->Get_MainPlayer();
             CheckNull(pPlayer);

             _vector vPos = pPlayer->Get_Transform()->Get_State(STATE::POSITION);
             _float4 pPlayerPos;
             XMStoreFloat4(&pPlayerPos, vPos);
             pPlayerPos.z -= 1.2f;
             CGameManager::GetInstance()->Set_LastPosition(pPlayerPos);

             m_pFadeScreen->Set_FadeInEndFunc([this]()
                 {
                     LevelArgs args;
                     args.iNextLevelID = ENUM_TO_UINT(LEVEL_ID::ROOM);
                     args.changeType = LEVELCHANGETYPE::PUSH;
                     args.loadingChangeType = LEVELCHANGETYPE::PUSH;
                     args.m_iLevelID = ENUM_TO_UINT(LEVEL_ID::LOADING);


                     if (FAILED(m_pGameInstance->Level_Changer(
                         ENUM_TO_UINT(LEVEL_ID::LOADING),
                         args)))
                         return;
                 });
             
             m_pFadeScreen->PlayFadeIn();
             }
             break;

             m_pGameInstance->Clear_SceneColliders(ENUM_TO_UINT(LEVEL_ID::ROOM));

    }
}

void CRoomTrigger::OnCollisionStay(_uint iGroup, CCollider_Base* pOther)
{
}

void CRoomTrigger::OnCollisionExit(_uint iGroup, CCollider_Base* pOther)
{
}

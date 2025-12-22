#include "CPlayerStateGetItem.h"
#include "CPlayer.h"
#include "CCamera_Base.h"
#include "GlobalGameEvent.h"
#include "CInventory_Manager.h"
#include "CMainCamera.h"
#include "CInput_Manager.h"


USING(Client)
CPlayerStateGetItem::CPlayerStateGetItem()
{
}

CPlayerStateGetItem::~CPlayerStateGetItem()
{
}

void CPlayerStateGetItem::Enter(CPlayer* pPlayer)
{
    pPlayerInput = pPlayer->Get_Input();
    pActionControl = pPlayer->Get_ActionControl();

    m_pMainCamera=dynamic_cast<CMainCamera*>(m_pGameInstance->Find_Camera(CAMERA_TYPE::TARGET));
    
    m_eGetItemType = CInventory_Manager::GetInstance()->Get_ItemGetEvent()->eType;

    //npc근처에있으면 이거 숨기기..
    m_pGameInstance->BroadCastEvent(L"OnTalkUIHide", nullptr);


    //최초로 얻은거라면, 노티파이 등록
    if (CInventory_Manager::GetInstance()->Get_InvenSize() == 0)
    {
        m_pGameInstance->RegisterListners("PlayerOnItemGet", [this](const GameEvent& event)
            {
                CPlayer* pPlayer = static_cast<CPlayer*>(event.Payload.Ptrs.at("Player"));
                if (pPlayer)
                {
                    if (!m_bShowUI)
                    {
                        CInventory_Manager::ItemGetEvent* pEvent = CInventory_Manager::GetInstance()->Get_ItemGetEvent();

                        m_pGameInstance->BroadCastEvent(L"OnItemGetIconShow", pEvent);
                        m_bShowUI = true;

                        m_pGameInstance->Invoke(2.f, 0.f, false, false, [pEvent]()
                            {
                                CGameInstance::GetInstance()->BroadCastEvent(L"OnItemDescUIShow", pEvent);
                                CGameInstance::GetInstance()->BroadCastEvent(L"UpdateItemDescText", pEvent);

                            },pPlayer);

                       
                       
                    }
                }



            });

        m_pGameInstance->RegisterListners("PlayerExitItemGet", [this](const GameEvent& event)
            {
                CPlayer* pPlayer = static_cast<CPlayer*>(event.Payload.Ptrs.at("Player"));
                if (pPlayer)
                {
                    if (m_bShowUI)
                    {
                        m_pGameInstance->BroadCastEvent(L"OnItemDescUIHide", nullptr);

                        m_pGameInstance->BroadCastEvent(L"OnItemGetIconHide", CInventory_Manager::GetInstance()->Get_ItemGetEvent());
                      
                        m_bShowUI = false;
                    }
                }



            });
    }
    m_ePhase = Phase::Start;
    m_eNextAnim = NextAnim::NONE;

    m_bChange = false;
    m_bChangeState = false;


    pPlayer->Set_HideWeapons();
    pPlayer->Set_CanMove(false);

    CCamera_Base* pCameraBase = m_pGameInstance->Get_MainCamera();
    pCameraBase->Set_Offset(_float3(0.f, 5.5f, -3.5f));

    m_pGameInstance->Invoke(0.3f, 0.f, false, false, [this,pPlayer]()
        {
            m_bStartAnim = true;
            pPlayer->Reserve_Animation_To_Body(L"item_get_st", false);

        },pPlayer);

    m_fTime = 0.f;

}

bool CPlayerStateGetItem::Update(CPlayer* pPlayer, _float fTimeDelta)
{
    CheckFalseResult(m_bStartAnim,false);
    if (m_bChange)
        ChangePhase(pPlayer);

    else if (m_bChangeState)
        ChangeState(pPlayer);

    return true;
}

void CPlayerStateGetItem::Update_Late(CPlayer* pPlayer, _float fTimeDelta)
{
    //먼저돌기
    pPlayer->Get_Transform()->LookAtSmooth(XMLoadFloat4(&m_pGameInstance->Get_CamPosition(ENUM_TO_UINT(CAMERA_TYPE::TARGET))), 10.f, fTimeDelta);
    CheckFalse(m_bStartAnim);

    //카메라 각도회전
    Change_CameraState(fTimeDelta);

    //페이즈 바꾸는 타이밍
    switch (m_ePhase)
    {
    case Phase::Start:
        if (pPlayer->Is_AnimEnd())
            m_bChange = true;
        break;


    case Phase::Loop:
    {
        if (CInput_Manager::GetInstance()->IsKeyPressed(KeyCode::A))
        {
            m_bChange = true;
            CInventory_Manager::GetInstance()->Set_End_in_SlotQueue(m_eGetItemType,true);
        }

        
    }
        break;

    case Phase::End:
        if (pPlayer->Is_AnimEnd())
        {
               
            m_bChangeState = true;
            m_eNextAnim = NextAnim::IDLE;
        }
        break;
    }
}

void CPlayerStateGetItem::Exit(CPlayer* pPlayer)
{
    pPlayer->Set_CanMove(true);
    pPlayer->Show_Weapons();

    pActionControl->m_bItemGet = false;
    

}

void CPlayerStateGetItem::ChangePhase(CPlayer* pPlayer)
{
    switch (m_ePhase)
    {
    case CPlayerStateGetItem::Phase::Start:
        pPlayer->Reserve_Animation_To_Body(L"item_get_lp", true);
        //카메라 줌인
        
        m_ePhase = Phase::Loop;
        break;

    case CPlayerStateGetItem::Phase::Loop:
    {   pPlayer->Reserve_Animation_To_Body(L"item_get_ed", false);
        m_ePhase = Phase::End;

        GameEvent	InitCameraEvent = MakeEvent("Init_Camera");
        EventPayload  Paylaod;
        Paylaod.Ptrs["Player"] = pPlayer;
        InitCameraEvent.Payload = Paylaod;
        m_pGameInstance->Emit(InitCameraEvent);


    }
        break;
      
     

    }
    m_bChange = false;
    
}

void CPlayerStateGetItem::ChangeState(CPlayer* pPlayer)
{
    switch (m_eNextAnim)
    {

    case Client::CPlayerStateGetItem::NextAnim::IDLE:
         pPlayer->Change_State(ENUM_TO_UINT(CPlayer::PLAYER_STATE::IDLE));
        break;
    default:
        break;
    }

    m_bChangeState = false;
}

void CPlayerStateGetItem::Change_CameraState(float _fTimeDelta)
{
    CheckNull(m_pMainCamera);

    if (m_ePhase != Phase::End)
    {
        _float3 vCurFloatRot = m_pMainCamera->Get_Transform()->Get_Rotation_ByEular();
        _float3 vCurFloatOffset = m_pMainCamera->Get_Offset();

        _vector vCurRot = XMLoadFloat3(&vCurFloatRot);
        _vector vCurOffSet = XMLoadFloat3(&vCurFloatOffset);

        _vector fRotation = XMVectorLerp(vCurRot, XMVectorSet(65.f, 0.f, 0.f, 1.f), 0.8f);
        _vector vOffset = XMVectorLerp(vCurOffSet, XMVectorSet(0.f, 6.5f, -2.5f, 0.f), 0.8f);

        _float4 vResult;
        _float3 fOffSet;
        XMStoreFloat4(&vResult, fRotation);
        XMStoreFloat3(&fOffSet, vOffset);

        m_pMainCamera->Set_LocalRoation(vResult);
        m_pMainCamera->Set_Offset(fOffSet);
    }

    else
    {
        _float3 vCurFloatRot = m_pMainCamera->Get_Transform()->Get_Rotation_ByEular();
        _float3 vCurFloatOffset = m_pMainCamera->Get_Offset();

        _vector vCurRot = XMLoadFloat3(&vCurFloatRot);
        _vector vCurOffSet = XMLoadFloat3(&vCurFloatOffset);

        
       
        _vector fRotation = XMVectorLerp(vCurRot,XMVectorSet(56.f,0.f,0.f,1.f), 0.8f);
        _vector vOffset = XMVectorLerp(vCurOffSet, XMLoadFloat3(&m_pMainCamera->Get_InitOffset()), 0.8f);

        _float4 vResult;
        _float3 fOffSet;
        XMStoreFloat4(&vResult, fRotation);
        XMStoreFloat3(&fOffSet, vOffset);

        m_pMainCamera->Set_LocalRoation(vResult);
        m_pMainCamera->Set_Offset(fOffSet);
    }
    
}

CPlayerStateGetItem* CPlayerStateGetItem::Create()
{
    return new CPlayerStateGetItem;
}

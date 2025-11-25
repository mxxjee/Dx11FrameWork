#include "CPlayerStateGetItem.h"
#include "CPlayer.h"
#include "CCamera_Base.h"
#include "GlobalGameEvent.h"

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

    pPlayer->Reserve_Animation_To_Body(L"item_get_st", false);

    m_ePhase = Phase::Start;
    m_eNextAnim = NextAnim::NONE;

    m_bChange = false;
    m_bChangeState = false;


    pPlayer->Set_HideWeapons();
    pPlayer->Set_CanMove(false);

    CCamera_Base* pCameraBase = m_pGameInstance->Get_MainCamera();
    pCameraBase->Set_Offset(_float3(0.f, 2.5f, -1.5f));



    m_fTime = 0.f;

}

void CPlayerStateGetItem::Update(CPlayer* pPlayer, _float fTimeDelta)
{
    if (m_bChange)
        ChangePhase(pPlayer);

    else if (m_bChangeState)
        ChangeState(pPlayer);
}

void CPlayerStateGetItem::Update_Late(CPlayer* pPlayer, _float fTimeDelta)
{
    //페이즈 바꾸는 타이밍
    switch (m_ePhase)
    {
    case Phase::Start:
        if (pPlayer->Is_AnimEnd())
            m_bChange = true;
        break;


    case Phase::Loop:
    {
        m_fTime += fTimeDelta;

        if (m_fTime >= 5.f)
        {
            m_bChange = true;
            m_fTime = 0.f;
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
    pPlayer->Set_Default();

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

CPlayerStateGetItem* CPlayerStateGetItem::Create()
{
    return new CPlayerStateGetItem;
}

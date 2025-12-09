#include "CPlayerPushState.h"
#include "CPlayer.h"

USING(Client)
CPlayerPushState::CPlayerPushState()
{
}

CPlayerPushState::~CPlayerPushState()
{
}

void CPlayerPushState::Enter(CPlayer* pPlayer)
{
    pPlayerInput = pPlayer->Get_Input();
    pActionControl = pPlayer->Get_ActionControl();

    m_ePrevDir = pPlayer->Get_CurDir();
    pPlayer->Reserve_Animation_To_Body(L"push_st", false);

    m_ePhase = Phase::Start;
    m_eNextAnim = NextAnim::NONE;

    m_bChange = false;
    m_bChangeState = false;


    pPlayer->Set_HideWeapons();


}

bool CPlayerPushState::Update(CPlayer* pPlayer, _float fTimeDelta)
{
    /*if (!pPlayerInput->m_bisMove || m_ePrevDir!=pPlayer->Get_CurDir())
    {
        return true;

    }*/

    Hold_Movement(pPlayer);

   

    if (pActionControl->m_bCarry)
    {
        pPlayer->Change_State(ENUM_TO_UINT(CPlayer::PLAYER_STATE::CARRY));
        return true;
    }

    if (m_bChange)
        ChangePhase(pPlayer);

    if (m_bChangeState)
        ChangeState(pPlayer);

    return true;
}

void CPlayerPushState::Update_Late(CPlayer* pPlayer, _float fTimeDelta)
{
    //페이즈 바꾸는 타이밍
    switch (m_ePhase)
    {

    case Phase::Start:
        if (pPlayer->Is_AnimEnd())
            m_bChange = true;
        break;
    }



}

void CPlayerPushState::Exit(CPlayer* pPlayer)
{
    pPlayer->Set_Default();
}

void CPlayerPushState::Hold_Movement(CPlayer* pPlayer)
{
    wstring Key = L"";
	switch (m_ePhase)
	{

	case Client::CPlayerPushState::Phase::Loop:
	{
        if (pPlayerInput->m_bisMove)
            Key = L"lp";

		else
            Key = L"wait";


	  pPlayer->Reserve_Animation_To_Body(L"push_" + Key, true);
    }
	break;

	default:
		break;
	}
    
}

void CPlayerPushState::ChangePhase(CPlayer* pPlayer)
{
    switch (m_ePhase)
    {
    case CPlayerPushState::Phase::Start:
        pPlayer->Reserve_Animation_To_Body(L"push_lp" , true);
        m_ePhase = Phase::Loop;
        break;

    }
}

void CPlayerPushState::ChangeState(CPlayer* pPlayer)
{
    switch (m_eNextAnim)
    {

    case Client::CPlayerPushState::NextAnim::IDLE:
        pPlayer->Change_State(ENUM_TO_UINT(CPlayer::PLAYER_STATE::IDLE));
        break;
    default:
        break;
    }
}

CPlayerPushState* CPlayerPushState::Create()
{
    return new CPlayerPushState;
}

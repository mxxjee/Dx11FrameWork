#include "CPlayerCarryState.h"
#include "CPlayer.h"

USING(Client)
CPlayerCarryState::CPlayerCarryState()
{
}

CPlayerCarryState::~CPlayerCarryState()
{
}

void CPlayerCarryState::Enter(CPlayer* pPlayer)
{
    pPlayerInput = pPlayer->Get_Input();
    pActionControl = pPlayer->Get_ActionControl();

    pPlayer->Reserve_Animation_To_Body(L"carry", false);

    pPlayer->Set_HideWeapons();

    m_ePhase = Phase::Start;
    m_eNextAnim = NextAnim::NONE;

    pPlayer->Set_CanMove(false);

}

void CPlayerCarryState::Update(CPlayer* pPlayer, _float fTimeDelta)
{
    if (m_bChange)
        ChangePhase(pPlayer);

    else if (m_bChangeState)
        ChangeState(pPlayer);
}

void CPlayerCarryState::Update_Late(CPlayer* pPlayer, _float fTimeDelta)
{
    Hold_Movement(pPlayer);

    //페이즈 바꾸는 타이밍
    switch (m_ePhase)
    {
    case Phase::Start:
        if (pPlayer->Is_AnimEnd())
            m_bChange = true;
        break;


    case Phase::Loop:
        if (!pActionControl->m_bCarry)
            m_bChange = true;
 
            

    break;

    case Phase::End:
        if (pPlayer->Is_AnimEnd())
        {

            m_bChangeState = true;
            m_eNextAnim = NextAnim::IDLE;
            pPlayer->Set_CanMove(true);
        }
        break;
    }
}

void CPlayerCarryState::Exit(CPlayer* pPlayer)
{
    pPlayer->Set_Default();

}

void CPlayerCarryState::Hold_Movement(CPlayer* pPlayer)
{

	switch (m_ePhase)
	{

	case Client::CPlayerCarryState::Phase::Loop:
	{
		if (pPlayerInput->m_bisMove)
            pPlayer->Reserve_Animation_To_Body(L"jump_carry", true);

		else
            pPlayer->Reserve_Animation_To_Body(L"idle_carry", true);

	}

	break;

	default:
		break;
	}
    
}

void CPlayerCarryState::ChangePhase(CPlayer* pPlayer)
{
    switch (m_ePhase)
    {
    case CPlayerCarryState::Phase::Start:
        pPlayer->Reserve_Animation_To_Body(L"idle_carry", true);
        pPlayer->Set_CanMove(true);
        m_ePhase = Phase::Loop;
        break;


    case CPlayerCarryState::Phase::Loop:
        {   
            pPlayer->Reserve_Animation_To_Body(L"throw", false);
            m_ePhase = Phase::End;
            pPlayer->Set_CanMove(false);

        }
    }
    m_bChange = false;
}

void CPlayerCarryState::ChangeState(CPlayer* pPlayer)
{
    switch (m_eNextAnim)
    {

    case Client::CPlayerCarryState::NextAnim::IDLE:
        pPlayer->Change_State(ENUM_TO_UINT(CPlayer::PLAYER_STATE::IDLE));
        break;
    default:
        break;
    }

    m_bChangeState = false;
}

CPlayerCarryState* CPlayerCarryState::Create()
{
    return new CPlayerCarryState;
}

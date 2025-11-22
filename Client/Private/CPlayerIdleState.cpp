#include "CPlayerIdleState.h"
#include "CPlayer.h"

USING(Client)
CPlayerIdleState::CPlayerIdleState()
{
}

CPlayerIdleState::~CPlayerIdleState()
{
}

void CPlayerIdleState::Enter(CPlayer* pPlayer)
{
	pPlayerInput = pPlayer->Get_Input();
	pPlayer->Reserve_Animation_To_Body(L"Idle", true);

	pPlayer->Reset_ActionControl();
	pPlayer->Set_CanMove(true);

}

void CPlayerIdleState::Update(CPlayer* pPlayer)
{
	if (pPlayerInput->m_bisAttack && pPlayer->Get_CanAttackEnable())
	{
		pPlayer->Change_State(ENUM_TO_UINT(CPlayer::PLAYER_STATE::ATTACK));
		return;
	}

	if (pPlayerInput->m_bisShield && pPlayer->Get_CanShieldEnable())
	{
		pPlayer->Change_State(ENUM_TO_UINT(CPlayer::PLAYER_STATE::HOLD_SHIELD));
		return;
	}


	else
	{
		if (pPlayerInput->m_bisJump)
			pPlayer->Change_State(ENUM_TO_UINT(CPlayer::PLAYER_STATE::JUMP));

		
		if (pPlayerInput->m_bisMove)
		{
			pPlayer->Change_State(ENUM_TO_UINT(CPlayer::PLAYER_STATE::RUN));

		}
			
	}

}

void CPlayerIdleState::Update_Late(CPlayer* pPlayer)
{
	//if (pPlayer->Get_FixDir())
	//	pPlayer->Set_FixDir(false);

}

void CPlayerIdleState::Exit(CPlayer* pPlayer)
{
}

CPlayerIdleState* CPlayerIdleState::Create()
{
	return new CPlayerIdleState;
}



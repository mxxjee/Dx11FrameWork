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
	pActionControl = pPlayer->Get_ActionControl();


	pPlayer->Reserve_Animation_To_Body(L"Idle", true);

	pPlayer->Reset_ActionControl();
	pPlayer->Set_CanMove(true);

}

void CPlayerIdleState::Update(CPlayer* pPlayer, _float fTimeDelta)
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

	if (pActionControl->m_bLadder )
	{
		pPlayer->Change_State(ENUM_TO_UINT(CPlayer::PLAYER_STATE::LADDER));
		return;
	}

	if (pActionControl->m_bPush)
	{
		pPlayer->Change_State(ENUM_TO_UINT(CPlayer::PLAYER_STATE::PUSH));
		return;
	}

	if (pActionControl->m_bItemGet)
	{
		pPlayer->Change_State(ENUM_TO_UINT(CPlayer::PLAYER_STATE::ITEMGET));
		return;
	}


	if (pActionControl->m_bCarry)
	{ 
		pPlayer->Change_State(ENUM_TO_UINT(CPlayer::PLAYER_STATE::CARRY));
		return;
	}



	if (pActionControl->m_bTalk)
	{
		pPlayer->Change_State(ENUM_TO_UINT(CPlayer::PLAYER_STATE::TALK));
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

void CPlayerIdleState::Update_Late(CPlayer* pPlayer, _float fTimeDelta)
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



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

	if (pPlayer->Get_PreState() == ENUM_TO_UINT(CPlayer::PLAYER_STATE::PUSH))
	{
		pPlayer->Reserve_Animation_To_Body(L"Idle", true, true);

	}
	else
		pPlayer->Reserve_Animation_To_Body(L"Idle", true);

	pPlayer->Reset_ActionControl();
	pPlayer->Set_CanMove(true);

}

bool CPlayerIdleState::Update(CPlayer* pPlayer, _float fTimeDelta)
{
	if (__super::Update(pPlayer, fTimeDelta))
		return false;

	if (pPlayer->Get_CurrentAnimKey() == L"Idle")
	{
		if (pActionControl->m_bPush)
			pActionControl->m_bPush = false;
	}

	if (pPlayerInput->m_bisAttack && pPlayer->Get_CanAttackEnable())
	{
		pPlayer->Change_State(ENUM_TO_UINT(CPlayer::PLAYER_STATE::ATTACK));
		return true;
	}

	if (pPlayerInput->m_bisShield && pPlayer->Get_CanShieldEnable())
	{
		pPlayer->Change_State(ENUM_TO_UINT(CPlayer::PLAYER_STATE::HOLD_SHIELD));
		return true;
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

	return true;

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



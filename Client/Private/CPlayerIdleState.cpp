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
	pPlayer->Reserve_Animation_To_Body(L"wait", true);
	pPlayer->Set_CanMove(true);

}

void CPlayerIdleState::Update(CPlayer* pPlayer)
{
	if (pPlayerInput->m_bisAttack && pPlayer->Get_CanAttackEnable())
	{
		pPlayer->Change_State(ENUM_TO_UINT(CPlayer::PLAYER_STATE::ATTACK));
		return;
	}


	else
	{
		if (pPlayerInput->m_bisMove)
			pPlayer->Change_State(ENUM_TO_UINT(CPlayer::PLAYER_STATE::RUN));

	}

}

void CPlayerIdleState::Update_Late(CPlayer* pPlayer)
{
	
}

void CPlayerIdleState::Exit(CPlayer* pPlayer)
{
}

CPlayerIdleState* CPlayerIdleState::Create()
{
	return new CPlayerIdleState;
}



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

}

void CPlayerIdleState::Update(CPlayer* pPlayer)
{
	if (pPlayerInput->m_bisAttack)
	{
		pPlayer->Change_State(CModelObject::ATTACK);
		return;
	}


	else
	{
		if (pPlayerInput->m_bisMove)
			pPlayer->Change_State(CModelObject::RUN);

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



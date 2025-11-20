#include "CPlayerRunState.h"
#include "CPlayer.h"

USING(Client)

CPlayerRunState::CPlayerRunState()
{
}

CPlayerRunState::~CPlayerRunState()
{
}

void CPlayerRunState::Enter(CPlayer* pPlayer)
{
	pPlayerInput = pPlayer->Get_Input();
	pPlayer->Reserve_Animation_To_Body(L"run", true);

}

void CPlayerRunState::Update(CPlayer* pPlayer)
{
	if (pPlayerInput->m_bisAttack )
		pPlayer->Change_State(CModelObject::ATTACK);

	else if (!pPlayerInput->m_bisMove)
		pPlayer->Change_State(CModelObject::IDLE);
}

void CPlayerRunState::Exit(CPlayer* pPlayer)
{
}

CPlayerRunState* CPlayerRunState::Create()
{
	return new CPlayerRunState;
}

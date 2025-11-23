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

	pPlayer->Reset_ActionControl();

}

void CPlayerRunState::Update(CPlayer* pPlayer, _float fTimeDelta)
{
	if (pPlayerInput->m_bisAttack && pPlayer->Get_CanAttackEnable())
		pPlayer->Change_State(ENUM_TO_UINT(CPlayer::PLAYER_STATE::ATTACK));

	if (pPlayerInput->m_bisShield && pPlayer->Get_CanShieldEnable())
		pPlayer->Change_State(ENUM_TO_UINT(CPlayer::PLAYER_STATE::HOLD_SHIELD));

	else
	{
		if (pPlayerInput->m_bisJump)
			pPlayer->Change_State(ENUM_TO_UINT(CPlayer::PLAYER_STATE::JUMP));

		if (!pPlayerInput->m_bisMove)
			pPlayer->Change_State(ENUM_TO_UINT(CPlayer::PLAYER_STATE::IDLE));

	}
}

void CPlayerRunState::Exit(CPlayer* pPlayer)
{
}

CPlayerRunState* CPlayerRunState::Create()
{
	return new CPlayerRunState;
}

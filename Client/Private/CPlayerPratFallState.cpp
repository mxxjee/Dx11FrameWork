#include "CPlayerPratFallState.h"
#include "CPlayer.h"
#include "CGameManager.h"

USING(Client)
CPlayerPratFallState::CPlayerPratFallState()
{
}

CPlayerPratFallState::~CPlayerPratFallState()
{
}

void CPlayerPratFallState::Enter(CPlayer* pPlayer)
{
	pPlayerInput = pPlayer->Get_Input();
	pActionControl = pPlayer->Get_ActionControl();

	m_CurPhase = PHASE::LP;

	pPlayer->Reserve_Animation_To_Body(L"ev_pratfall_lp", true);
	pPlayer->Set_CanMove(false);

}

bool CPlayerPratFallState::Update(CPlayer* pPlayer, _float fTimeDelta)
{
	if (m_CurPhase == PHASE::LP && !CGameManager::GetInstance()->Get_UseCutScene())
	{
		pPlayer->Reserve_Animation_To_Body(L"ev_pratfall_ed", false);
		m_CurPhase = PHASE::END;

	}

	if (m_CurPhase == PHASE::END && pPlayer->Is_AnimEnd())
	{
		pPlayer->Change_State(ENUM_TO_UINT(CPlayer::PLAYER_STATE::IDLE));


	}
	return false;
}

void CPlayerPratFallState::Update_Late(CPlayer* pPlayer, _float fTimeDelta)
{
}

void CPlayerPratFallState::Exit(CPlayer* pPlayer)
{

}

CPlayerPratFallState* CPlayerPratFallState::Create()
{
    return new CPlayerPratFallState;
}

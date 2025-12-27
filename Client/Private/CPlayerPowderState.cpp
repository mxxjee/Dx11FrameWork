#include "CPlayerPowderState.h"
#include "CPlayer.h"


USING(Client)
CPlayerPowderState::CPlayerPowderState()
{
}

CPlayerPowderState::~CPlayerPowderState()
{
}

void CPlayerPowderState::Enter(CPlayer* pPlayer)
{

	pPlayerInput = pPlayer->Get_Input();
	pActionControl = pPlayer->Get_ActionControl();


	pPlayer->Reserve_Animation_To_Body(L"powder", false);


	pPlayer->Set_CanMove(false);
}

bool CPlayerPowderState::Update(CPlayer* pPlayer, _float fTimeDelta)
{
	if (pPlayer->Is_AnimEnd())
	{
		pPlayer->Change_State(ENUM_TO_UINT(CPlayer::PLAYER_STATE::IDLE));
		return true;
	}

	return true;
}

void CPlayerPowderState::Update_Late(CPlayer* pPlayer, _float fTimeDelta)
{
}

void CPlayerPowderState::Exit(CPlayer* pPlayer)
{
}

CPlayerPowderState* CPlayerPowderState::Create()
{
	return new CPlayerPowderState;
}

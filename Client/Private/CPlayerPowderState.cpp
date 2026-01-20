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

	pPlayer->Set_HideWeapons();

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
	if(m_pGameInstance->Get_CurrentLevelID()!=ENUM_TO_UINT(LEVEL_ID::ROOM))
		pPlayer->Show_Weapons();
}

CPlayerPowderState* CPlayerPowderState::Create()
{
	return new CPlayerPowderState;
}

#include "CPlayerState.h"
#include "CPlayer.h"
#include "CGameInstance.h"

USING(Client)
CPlayerState::CPlayerState()
	:m_pGameInstance(CGameInstance::GetInstance())
{
	
}

bool CPlayerState::Update(CPlayer* pPlayer, _float fTimeDelta)
{
	if (pPlayer->Get_DamageRender()==1.f)
	{
		pPlayer->Change_State(ENUM_TO_UINT(CPlayer::PLAYER_STATE::DAMANGE));
		return true;
	}

	if (pActionControl->m_bLadder)
	{
		pPlayer->Change_State(ENUM_TO_UINT(CPlayer::PLAYER_STATE::LADDER));
		return true;
	}

	

	if (pActionControl->m_bItemGet)
	{
		pPlayer->Change_State(ENUM_TO_UINT(CPlayer::PLAYER_STATE::ITEMGET));
		return true;
	}


	if (pActionControl->m_bCarry)
	{
		pPlayer->Change_State(ENUM_TO_UINT(CPlayer::PLAYER_STATE::CARRY));
		return true;
	}



	if (pActionControl->m_bTalk)
	{
		pPlayer->Change_State(ENUM_TO_UINT(CPlayer::PLAYER_STATE::TALK));
		return true;
	}

	if (pActionControl->m_bFall)
	{
		pPlayer->Change_State(ENUM_TO_UINT(CPlayer::PLAYER_STATE::FALL));
		return true;
	}

	return false;
}

void CPlayerState::Free()
{
}

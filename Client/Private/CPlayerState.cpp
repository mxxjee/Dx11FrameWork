#include "CPlayerState.h"
#include "CPlayer.h"
#include "CGameInstance.h"

USING(Client)
CPlayerState::CPlayerState()
	:m_pGameInstance(CGameInstance::GetInstance())
{
	
}

void CPlayerState::Update(CPlayer* pPlayer, _float fTimeDelta)
{
	if (pActionControl->m_bLadder)
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

	if (pActionControl->m_bFall)
	{
		pPlayer->Change_State(ENUM_TO_UINT(CPlayer::PLAYER_STATE::FALL));
		return;
	}
}

void CPlayerState::Free()
{
}

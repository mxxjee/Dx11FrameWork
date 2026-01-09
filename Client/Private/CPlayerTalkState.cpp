#include "CPlayerTalkState.h"
#include "CPlayer.h"

USING(Client)
CPlayerTalkState::CPlayerTalkState()
{
}

CPlayerTalkState::~CPlayerTalkState()
{
}

void CPlayerTalkState::Enter(CPlayer* pPlayer)
{
    
	pPlayerInput = pPlayer->Get_Input();
	pActionControl = pPlayer->Get_ActionControl();


	pPlayer->Reserve_Animation_To_Body(L"talk", true);


	pPlayer->Set_CanMove(false);
}

bool CPlayerTalkState::Update(CPlayer* pPlayer, _float fTimeDelta)
{
	if(pPlayer->Get_Target())
		pPlayer->Get_Transform()->LookAtSmooth(pPlayer->Get_Target()->Get_Transform()->Get_State(STATE::POSITION), 5.f, fTimeDelta);


	if (!pActionControl->m_bTalk)
	{
		pPlayer->Change_State(ENUM_TO_UINT(CPlayer::PLAYER_STATE::IDLE));
		return true;
	}

	return true;
}

void CPlayerTalkState::Update_Late(CPlayer* pPlayer, _float fTimeDelta)
{
	
}


void CPlayerTalkState::Exit(CPlayer* pPlayer)
{
}

CPlayerTalkState* CPlayerTalkState::Create()
{
    return new CPlayerTalkState;
}

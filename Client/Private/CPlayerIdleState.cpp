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
	pPlayer->Reserve_Animation_To_Body(L"wait", true);

}

void CPlayerIdleState::Update(CPlayer* pPlayer)
{
	
}

void CPlayerIdleState::Exit(CPlayer* pPlayer)
{
}



#include "CPlayerHoldShield.h"
#include "CPlayer.h"

USING(Client)

CPlayerHoldShield::CPlayerHoldShield()
{
}

CPlayerHoldShield::~CPlayerHoldShield()
{
}

void CPlayerHoldShield::Enter(CPlayer* pPlayer)
{
    pPlayerInput = pPlayer->Get_Input();
    pPlayer->Reserve_Animation_To_Body(L"slash_hold_st", false);


}

void CPlayerHoldShield::Update(CPlayer* pPlayer)
{
}

void CPlayerHoldShield::Update_Late(CPlayer* pPlayer)
{
}

void CPlayerHoldShield::Exit(CPlayer* pPlayer)
{
}

CPlayerHoldShield* CPlayerHoldShield::Create()
{
    return new CPlayerHoldShield;
}

#include "CPlayerUpStairState.h"
#include "CPlayer.h"

USING(Client)
CPlayerUpStairState::CPlayerUpStairState()
{
}

CPlayerUpStairState::~CPlayerUpStairState()
{
}

void CPlayerUpStairState::Enter(CPlayer* pPlayer)
{
    pPlayerInput = pPlayer->Get_Input();
    pActionControl = pPlayer->Get_ActionControl();

    pPlayer->Reserve_Animation_To_Body(L"ev_stairs_up",false,true);

}

bool CPlayerUpStairState::Update(CPlayer* pPlayer, _float fTimeDelta)
{
    if (pPlayer->Is_AnimEnd())
        int A = 10;
    return true;
}

void CPlayerUpStairState::Update_Late(CPlayer* pPlayer, _float fTimeDelta)
{
}

void CPlayerUpStairState::Exit(CPlayer* pPlayer)
{
}

CPlayerUpStairState* CPlayerUpStairState::Create()
{
    return new CPlayerUpStairState;
}

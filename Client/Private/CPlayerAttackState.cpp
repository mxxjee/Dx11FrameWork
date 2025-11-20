#include "CPlayerAttackState.h"
#include "CPlayer.h"
#include "CInput_Manager.h"

USING(Client)

CPlayerAttackState::CPlayerAttackState()
{
}

CPlayerAttackState::~CPlayerAttackState()
{
}

void CPlayerAttackState::Enter(CPlayer* pPlayer)
{
    pPlayerInput = pPlayer->Get_Input();
    pPlayer->Reserve_Animation_To_Body(L"slash", false);

    //중복공격막고..
    pPlayer->Set_CanAttackEnable(false);
    pPlayer->Set_CanMove(false);
    m_bChange = false;
}


void CPlayerAttackState::Update(CPlayer* pPlayer)
{
    if (m_bChange)
    {
        pPlayer->Change_State(CModelObject::IDLE);
        pPlayer->Set_CanAttackEnable(true);
        pPlayer->Set_CanMove(true);
    }
       

}

void CPlayerAttackState::Update_Late(CPlayer* pPlayer)
{
    if (pPlayer->Is_AnimEnd())
    {
        m_bChange = true;
       
    }
}

void CPlayerAttackState::Exit(CPlayer* pPlayer)
{
   
}

CPlayerAttackState* CPlayerAttackState::Create()
{
    return new CPlayerAttackState;
}

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
        switch (e_NextAnim)
        {
        case NextAnim::IDLE:
            pPlayer->Change_State(ENUM_TO_UINT(CPlayer::PLAYER_STATE::IDLE));
  
            break;

        case NextAnim::HOLD:
            pPlayer->Change_State(ENUM_TO_UINT(CPlayer::PLAYER_STATE::HOLD_ATTACK));

        break;
        }
       

        pPlayer->Set_CanAttackEnable(true);
    }
       

}

void CPlayerAttackState::Update_Late(CPlayer* pPlayer)
{
    if (pPlayer->Is_AnimEnd() && !pPlayerInput->m_bisAttack)
    {
        e_NextAnim = NextAnim::IDLE;
        m_bChange = true;
       
    }

    else if (pPlayer->Is_AnimEnd() && pPlayerInput->m_bisAttack)
    {
        e_NextAnim = NextAnim::HOLD;
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

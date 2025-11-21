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


    e_NextAnim = NextAnim::NONE;

    pPlayer->Reset_ActionControl();

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
       
    }
       

}

void CPlayerAttackState::Update_Late(CPlayer* pPlayer)
{
    CheckFalse(e_NextAnim == NextAnim::NONE);

    //홀드왜들어옴..? 
    if (pPlayer->Is_AnimEnd() && pPlayer->Get_Hold(CPlayer::HOLD_B))
    {
        e_NextAnim = NextAnim::HOLD;
        m_bChange = true;
    }

    else if (pPlayer->Is_AnimEnd() && !pPlayerInput->m_bisAttack )
    {
        e_NextAnim = NextAnim::IDLE;
        m_bChange = true;
        pPlayer->Set_CanAttackEnable(false);
    }
  
}

void CPlayerAttackState::Exit(CPlayer* pPlayer)
{
   
}

CPlayerAttackState* CPlayerAttackState::Create()
{
    return new CPlayerAttackState;
}

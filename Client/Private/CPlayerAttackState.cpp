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


void CPlayerAttackState::Update(CPlayer* pPlayer, _float fTimeDelta)
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
        
        case NextAnim::SHIELD:
            pPlayer->Change_State(ENUM_TO_UINT(CPlayer::PLAYER_STATE::HOLD_SHIELD));
            break;

        case NextAnim::SLASH_SHIELD:
            pPlayer->Change_State(ENUM_TO_UINT(CPlayer::PLAYER_STATE::SLASH_SHIELD));
            break;
        }
       
    }
       

}

void CPlayerAttackState::Update_Late(CPlayer* pPlayer, _float fTimeDelta)
{
    CheckFalse(e_NextAnim == NextAnim::NONE);

    //b키르누르고있는데
    if (pPlayer->Is_AnimEnd() && pPlayer->Get_Hold(CPlayer::HOLD_B))
    {
                //쉴드키까지눌리면 합성상태
        if(pPlayer->Get_Hold(CPlayer::HoldKey::HOLD_T))
            e_NextAnim = NextAnim::SLASH_SHIELD;

        //아니면 홀드
        else
            e_NextAnim = NextAnim::HOLD;
        
        m_bChange = true;
        pPlayer->Set_CanAttackEnable(true);
    }

    //else if (pPlayer->Is_AnimEnd() && pPlayer->Get_Hold(CPlayer::HoldKey::HOLD_T))
    //{
    //    if (pPlayer->Get_Hold(CPlayer::HoldKey::HOLD_B))
    //        e_NextAnim = NextAnim::SLASH_SHIELD;

    //    else
    //        e_NextAnim = NextAnim::SHIELD;

    //    m_bChange = true;
    //    pPlayer->Set_CanAttackEnable(true);
    //}


    //한번만 딱 눌렀을떄.
    else if (pPlayer->Is_AnimEnd() && !pPlayerInput->m_bisAttack )
    {
        if(pPlayer->Get_Hold(CPlayer::HoldKey::HOLD_T))
            e_NextAnim = NextAnim::IDLE;

        else
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

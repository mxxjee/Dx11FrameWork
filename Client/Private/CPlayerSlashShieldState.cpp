#include "CPlayerSlashShieldState.h"
#include "CPlayer.h"


USING(Client)
CPlayerSlashShieldState::CPlayerSlashShieldState()
{
}

CPlayerSlashShieldState::~CPlayerSlashShieldState()
{
}

void CPlayerSlashShieldState::Enter(CPlayer* pPlayer)
{
    pPlayerInput = pPlayer->Get_Input();
    pPlayer->Reserve_Animation_To_Body(L"slash_hold_shield_lp", true);
    
    pPlayer->Reset_ActionControl();
    pPlayer->Set_CanMove(true);

    e_NextAnim = NextAnim::NONE;
    m_ePhase = Phase::Loop;


    m_bChangeState = false; //state바꾸기


}

void CPlayerSlashShieldState::Update(CPlayer* pPlayer, _float fTimeDelta)
{
    Hold_Movement(pPlayer);


   if (m_bChangeState)
        Change_Other_State(pPlayer);


}

void CPlayerSlashShieldState::Update_Late(CPlayer* pPlayer, _float fTimeDelta)
{

    bool m_bHoldT = pPlayer->Get_Hold(CPlayer::HoldKey::HOLD_T);
    bool m_bHoldB = pPlayer->Get_Hold(CPlayer::HoldKey::HOLD_B);

    //키를다땟을떄->slash_hold_ed
    if (!m_bHoldB && !pPlayerInput->m_bisAttack)
    {
        e_NextAnim = NextAnim::ATTACK_END;
        m_bChangeState = true;
    }

    //b는누르고욌는데 T만 덌을떄
    else if (m_bHoldB&&!m_bHoldT && !pPlayerInput->m_bisShieldRelease)
    {
        e_NextAnim = NextAnim::ATTACK_HOLD;
        m_bChangeState = true;
    }

        
    switch (e_NextAnim)
    {
    case NextAnim::ATTACK_END:
    {
        if (pPlayer->Is_AnimEnd())
            e_NextAnim = NextAnim::IDLE;

        m_bChangeState = true;
    }
        break;
    }

}

void CPlayerSlashShieldState::Exit(CPlayer* pPlayer)
{
}

void CPlayerSlashShieldState::Hold_Movement(CPlayer* pPlayer)
{
    //방향에 따른 애니메이션 적용
 //방향에 따른 애니메이션 적용
    if (pPlayer->Get_Transform())
    {
        CTransform* pTransform = pPlayer->Get_Transform();

        wstring dir = L"";
        _vector MoveDir = pTransform->Get_MoveDir();

        _float4 fMoveDir;
        XMStoreFloat4(&fMoveDir, MoveDir);

        _vector vRight = pPlayer->Get_Transform()->Get_State(STATE::RIGHT);
        _vector look = pPlayer->Get_Transform()->Get_State(STATE::LOOK);



        //움직인 축이 현재 내 look,right와 얼마나 일치했는지 판별하기
       //Input 방향이 플레이어 기준 앞인지?
        float forwardDot = XMVectorGetX(XMVector3Dot(look, MoveDir));
        float RightDot = XMVectorGetX(XMVector3Dot(vRight, MoveDir));

        switch (m_ePhase)
        {

        case Client::CPlayerSlashShieldState::Phase::Loop:
        {
            if (pPlayerInput->m_bisMove)
            {
                if (fabs(RightDot) > fabs(forwardDot))
                {
                    if (RightDot > 0)
                        dir = L"r";
                    else
                        dir = L"l";
                }

                else
                {
                    if (forwardDot > 0)
                        dir = L"f";
                    else
                        dir = L"b";
                }
            }

            else
                dir = L"lp";
        }

        pPlayer->Reserve_Animation_To_Body(L"slash_hold_shield_" + dir, true);
        break;

        default:
            break;
        }
    }
}

void CPlayerSlashShieldState::Change_Other_State(CPlayer* pPlayer)
{
    switch (e_NextAnim)
    {
    case NextAnim::ATTACK_END:
        pPlayer->Reserve_Animation_To_Body(L"slash_hold_ed", false);
        break;


    case NextAnim::IDLE:
        pPlayer->Change_State(ENUM_TO_UINT(CPlayer::PLAYER_STATE::IDLE));
        break;


    case NextAnim::SHIELD:
        pPlayer->Change_State(ENUM_TO_UINT(CPlayer::PLAYER_STATE::HOLD_SHIELD));
        break;


    case NextAnim::ATTACK:
        pPlayer->Change_State(ENUM_TO_UINT(CPlayer::PLAYER_STATE::ATTACK));
        break;

    case NextAnim::ATTACK_HOLD:
        pPlayer->Change_State(ENUM_TO_UINT(CPlayer::PLAYER_STATE::HOLD_ATTACK));
        break;
    }

    m_bChangeState = false;
}

void CPlayerSlashShieldState::Change_Phase(CPlayer* pPlayer)
{
}

CPlayerSlashShieldState* CPlayerSlashShieldState::Create()
{
    return new CPlayerSlashShieldState;
}

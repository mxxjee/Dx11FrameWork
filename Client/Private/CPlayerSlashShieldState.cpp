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
}

void CPlayerSlashShieldState::Update(CPlayer* pPlayer)
{
    Hold_Movement(pPlayer);

}

void CPlayerSlashShieldState::Update_Late(CPlayer* pPlayer)
{
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

CPlayerSlashShieldState* CPlayerSlashShieldState::Create()
{
    return new CPlayerSlashShieldState;
}

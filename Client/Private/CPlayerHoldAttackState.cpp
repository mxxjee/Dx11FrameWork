#include "CPlayerHoldAttackState.h"
#include "CPlayer.h"

USING(Client)

CPlayerHoldAttackState::CPlayerHoldAttackState()
{
}

CPlayerHoldAttackState::~CPlayerHoldAttackState()
{
}

void CPlayerHoldAttackState::Enter(CPlayer* pPlayer)
{
    pPlayerInput = pPlayer->Get_Input();
    pPlayer->Reserve_Animation_To_Body(L"slash_hold_st", false);

    pPlayer->Reset_ActionControl();

    //중복공격막고..
    pPlayer->Set_CanAttackEnable(true);
    pPlayer->Set_CanMove(false);
    pPlayer->Set_FixDir(true);
    m_ePhase= Phase::Start;
}

void CPlayerHoldAttackState::Update(CPlayer* pPlayer)
{

    //Hold상태일 시 움직이는 애니메이션에 따른 처리
    Hold_Movement(pPlayer);

    switch (m_ePhase)
    {
        //현재상태가 Start인데, 바꾸라는명령을 받았다.
    case Client::CPlayerHoldAttackState::Phase::Start:
    {
        if (m_bChange)
        {
            pPlayer->Set_CanMove(true);
            m_ePhase = Phase::Loop;
            pPlayer->Reserve_Animation_To_Body(L"slash_hold_lp", true);
            m_bChange = false;

        }
    }
    break;
   
            //현재상태가 Loop인데, 바꾸라는명령을 받았다.
    case Client::CPlayerHoldAttackState::Phase::Loop:
    {
         if (m_bChange)  //EndAnim으로바꾸기
        {
            m_ePhase = Phase::End;
            pPlayer->Reserve_Animation_To_Body(L"slash_hold_ed", false);
            m_bChange = false;
            pPlayer->Set_CanMove(false);

            
        }

    }
        break;
    case Client::CPlayerHoldAttackState::Phase::End:
    {
        if (m_bChange)
        {

            pPlayer->Change_State(ENUM_TO_UINT(CPlayer::PLAYER_STATE::IDLE));
            pPlayer->Set_CanMove(true);
            pPlayer->Set_FixDir(true);
            m_bChange = false;
            

        }
    }
        break;
    default:
        break;
    }

}

void CPlayerHoldAttackState::Update_Late(CPlayer* pPlayer)
{

    //바꾸라는 명령을 줄 타이밍
	switch (m_ePhase)
	{
	case Phase::Start:
    case Phase::End:
        if (pPlayer->Is_AnimEnd())
        {
            m_bChange = true;
 

        }
		break;


	case Phase::Loop:
        if (!pPlayerInput->m_bisAttack)
        {
            m_bChange = true;
        }
		break;


	default:
		break;
	}

}

void CPlayerHoldAttackState::Exit(CPlayer* pPlayer)
{
}

void CPlayerHoldAttackState::Hold_Movement(CPlayer* pPlayer)
{
    //방향에 따른 애니메이션 적용
    if (pPlayer->Get_Transform())
    {
        wstring dir = L"";
        _vector MoveDir = pPlayer->Get_Transform()->Get_MoveDir();
        _float4 fMoveDir;

        XMStoreFloat4(&fMoveDir, MoveDir);
        switch (m_ePhase)
        {

        case Client::CPlayerHoldAttackState::Phase::Loop:
        {
            if (pPlayerInput->m_bisMove)
            {
                if (fMoveDir.x < 0)
                    dir = L"l";

                else if (fMoveDir.x > 0)
                    dir = L"r";

                else if (fMoveDir.z > 0)
                    dir = L"f";

                else if (fMoveDir.z < 0)
                    dir = L"b";
            }

            else
                dir = L"lp";
        }

            pPlayer->Reserve_Animation_To_Body(L"slash_hold_"+dir, true);
            break;

        default:
            break;
        }
    }
}

CPlayerHoldAttackState* CPlayerHoldAttackState::Create()
{
    return new CPlayerHoldAttackState;
}

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
    pPlayer->Set_CanMove(true);
    pPlayer->Set_FixDir(true);

    m_ePhase= Phase::Loop;
    m_bChange = false;
    m_bChangeState = false;

    m_eNextAnim = NextAnim::NONE;


    m_pGameInstance->Invoke(1.f, 0.f, false, false, [this,pPlayer]()
        {
            OnChargeCompleteEnter(pPlayer);
        },pPlayer);
}

bool CPlayerHoldAttackState::Update(CPlayer* pPlayer, _float fTimeDelta)
{

   



    //Hold상태일 시 움직이는 애니메이션에 따른 처리
    Hold_Movement(pPlayer);


    if (m_bChange)
        ChangePhase(pPlayer);


    else if (m_bChangeState)
        ChangeState(pPlayer);


    return true;
}

void CPlayerHoldAttackState::Update_Late(CPlayer* pPlayer, _float fTimeDelta)
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
            m_bChange = true;


        else if (pPlayerInput->m_bisShield)
        {
            m_bChangeState = true;
            m_eNextAnim = NextAnim::SLASH_SHIELD;
        }
		break;


	default:
		break;
	}
    
}

void CPlayerHoldAttackState::Exit(CPlayer* pPlayer)
{
   
}

void CPlayerHoldAttackState::ChangePhase(CPlayer* pPlayer)
{

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
            pPlayer->OnAttackEnd();
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
    m_bChange = false;
}

void CPlayerHoldAttackState::ChangeState(CPlayer* pPlayer)
{
    switch (m_eNextAnim)
    {
    case NextAnim::SLASH_SHIELD:
        pPlayer->Change_State(ENUM_TO_UINT(CPlayer::PLAYER_STATE::SLASH_SHIELD));
        break;
    }

    m_bChangeState = false;
}

void CPlayerHoldAttackState::OnChargeCompleteEnter(CPlayer* pPlayer)
{
    //이펙트소환
    pPlayer->AnimNotify_End(PLAYER_ANIMNOTIFY_TYPE::SLASH_HOLD_ST_END);

}

void CPlayerHoldAttackState::Hold_Movement(CPlayer* pPlayer)
{
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

        case Client::CPlayerHoldAttackState::Phase::Loop:
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

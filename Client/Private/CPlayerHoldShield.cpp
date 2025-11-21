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
    pPlayer->Reserve_Animation_To_Body(L"shield_lp", false);

    e_NextAnim = NextAnim::NONE;
    m_ePhase = Phase::Loop;


    m_bChangePhase = false; 
    m_bChangeState = false;
    m_bExitState = false;
    pPlayer->Reset_ActionControl();


}

void CPlayerHoldShield::Update(CPlayer* pPlayer)
{
    Hold_Movement(pPlayer);

    if (m_bChangePhase)
        Change_Phase(pPlayer);

    else if (m_bChangeState)
        Change_Other_State(pPlayer);

   
}

void CPlayerHoldShield::Update_Late(CPlayer* pPlayer)
{
  

    bool m_bHoldT = pPlayer->Get_Hold(CPlayer::HoldKey::HOLD_T);
    bool m_bHoldB = pPlayer->Get_Hold(CPlayer::HoldKey::HOLD_B);

    //T키 홀딩중일떄..
    if (m_bHoldT)
    {
        //if문 추가(B키 같이눌림 들어왓을떄)
        

        //쉴드 단일동작들..
        switch (m_ePhase)
        {
        case Client::CPlayerHoldShield::Phase::Start:
            if (pPlayer->Is_AnimEnd())
                m_bChangePhase = true;
            break;

        case Client::CPlayerHoldShield::Phase::Loop:
            if (pPlayerInput->m_bisShieldRelease)
            {
                if (pPlayerInput->m_bisMove)
                    m_bChangeState = true;

                else
                    m_bChangePhase = false;
                
            }

                

            break;

        case Client::CPlayerHoldShield::Phase::End:
            break;

        default:
            break;
        }

           
    }


    //T 홀딩중아닐떄..
    else
    {

        //반드시 ed로 가게 해서 끝처리
        if (!pPlayerInput->m_bisShield)
        {
            switch (m_ePhase)
            {
            case Client::CPlayerHoldShield::Phase::Start:
                if (pPlayer->Is_AnimEnd())
                    m_bChangePhase = true;

                break;

            case Client::CPlayerHoldShield::Phase::Loop:
            {
                if (pPlayerInput->m_bisShieldRelease)
                {
                    if (pPlayerInput->m_bisMove)
                    {
                        m_bChangeState = true;
                        m_bRunMode = true;
                    }

                    else
                        m_bChangePhase = false;

                }


            }
                break;

            case Client::CPlayerHoldShield::Phase::End:
                if (pPlayer->Is_AnimEnd())
                    m_bChangePhase = true;
                break;
            default:
                break;
            }
        }
    }




}

void CPlayerHoldShield::Change_Other_State(CPlayer* pPlayer)
{
    switch (m_ePhase)
    {
  
    case Client::CPlayerHoldShield::Phase::Loop:
        if(m_bRunMode)
            pPlayer->Change_State(ENUM_TO_UINT(CPlayer::PLAYER_STATE::RUN));

        break;

    default:
        break;
    }
 
    m_bChangeState = false;
}

void CPlayerHoldShield::Change_Phase(CPlayer* pPlayer)
{
	//if(m_bChangePhase)


	switch (m_ePhase)
	{
	case Phase::Start:  //start->Loop로바껴라
	{
		m_ePhase = Phase::Loop;
		pPlayer->Reserve_Animation_To_Body(L"shield_lp", false);
	}
	break;

	case Phase::Loop:       //Loop->end로바껴라
	{
		m_ePhase = Phase::End;
		pPlayer->Reserve_Animation_To_Body(L"shield_ed", false);
	}
	break;

	case Phase::End:           //End탈출해라.
		pPlayer->Change_State(ENUM_TO_UINT(CPlayer::PLAYER_STATE::IDLE));

		break;
	}

    m_bChangePhase = false;
}

void CPlayerHoldShield::Hold_Movement(CPlayer* pPlayer)
{
    //방향에 따른 애니메이션 적용


    switch (m_ePhase)
    {

    case Client::CPlayerHoldShield::Phase::Loop:
    {
        if (pPlayerInput->m_bisMove)
            pPlayer->Reserve_Animation_To_Body(L"shield_hold_f", true);

        else
            pPlayer->Reserve_Animation_To_Body(L"shield_lp", true);

    }


    break;

    default:
        break;

    }
}

void CPlayerHoldShield::Exit(CPlayer* pPlayer)
{

    m_bChangeState = false;
}



CPlayerHoldShield* CPlayerHoldShield::Create()
{
    return new CPlayerHoldShield;
}

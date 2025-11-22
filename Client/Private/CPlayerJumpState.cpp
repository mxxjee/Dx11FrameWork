#include "CPlayerJumpState.h"
#include "CPlayer.h"

USING(Client)
CPlayerJumpState::CPlayerJumpState()
{
}

CPlayerJumpState::~CPlayerJumpState()
{
}

void CPlayerJumpState::Enter(CPlayer* pPlayer)
{

    pPlayerInput = pPlayer->Get_Input();
    pPlayer->Reserve_Animation_To_Body(L"jump", false);

    pPlayer->Reset_ActionControl();


    m_ePhase = Phase::Start;
    m_eNextAnim = NextAnim::NONE;

    m_bChange = false;
    m_bChangeState = false;
}

void CPlayerJumpState::Update(CPlayer* pPlayer)
{
    if (m_bChange)
        ChangePhase(pPlayer);


    else if (m_bChangeState)
        ChangeState(pPlayer);

}

void CPlayerJumpState::Update_Late(CPlayer* pPlayer)
{
    //페이즈 바꾸는 타이밍
    switch (m_ePhase)
    {
    case Phase::Start:
        if (pPlayer->Is_AnimEnd())
            m_bChange = true;

        break;


    case Phase::End:
        if (pPlayer->Is_AnimEnd())
        {
            m_bChangeState = true;
            m_eNextAnim = NextAnim::IDLE;

        }
        break;
    }
}

void CPlayerJumpState::Exit(CPlayer* pPlayer)
{
}

void CPlayerJumpState::ChangePhase(CPlayer* pPlayer)
{
    switch (m_ePhase)
    {
    case Phase::Start:
    {
        pPlayer->Reserve_Animation_To_Body(L"land",false);
        m_ePhase = Phase::End;
    }


        break;

    }

    m_bChange = false;
}

void CPlayerJumpState::ChangeState(CPlayer* pPlayer)
{
    switch (m_ePhase)
    {
    case Phase::End:
        pPlayer->Change_State(ENUM_TO_UINT(CPlayer::PLAYER_STATE::IDLE));

    break;

    }

    m_bChangeState = false;
}

CPlayerJumpState* CPlayerJumpState::Create()
{
	return new CPlayerJumpState;
}

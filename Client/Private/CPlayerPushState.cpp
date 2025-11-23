#include "CPlayerPushState.h"
#include "CPlayer.h"

USING(Client)
CPlayerPushState::CPlayerPushState()
{
}

CPlayerPushState::~CPlayerPushState()
{
}

void CPlayerPushState::Enter(CPlayer* pPlayer)
{
    pPlayerInput = pPlayer->Get_Input();
    pActionControl = pPlayer->Get_ActionControl();

    pPlayer->Reserve_Animation_To_Body(L"push_st", false);

    m_ePhase = Phase::Start;
    m_eNextAnim = NextAnim::NONE;

    m_bChange = false;
    m_bChangeState = false;


    pPlayer->Set_HideWeapons();


}

void CPlayerPushState::Update(CPlayer* pPlayer, _float fTimeDelta)
{
    Hold_Movement(pPlayer);


    if (pActionControl->m_bCarry)
    {
        pPlayer->Change_State(ENUM_TO_UINT(CPlayer::PLAYER_STATE::CARRY));
        return;
    }

    if (m_bChange)
        ChangePhase(pPlayer);

    if (m_bChangeState)
        ChangeState(pPlayer);
}

void CPlayerPushState::Update_Late(CPlayer* pPlayer, _float fTimeDelta)
{
    //페이즈 바꾸는 타이밍
    switch (m_ePhase)
    {

    case Phase::Start:
        if (pPlayer->Is_AnimEnd())
            m_bChange = true;
        break;
    }


    //스테이트 바꾸는 타이밍
    switch (m_ePhase)
    {

        //루프일떄,
    case Phase::Loop:
        //원래는 키보드 떼면 탈출 or push오브젝트와 충돌하지않았을떄 or 완료싸인받았을떄(이벤트라서 끝남을 알릴수잉씀)
        if (!pActionControl->m_bPush )
        {
            m_bChangeState = true;
            m_eNextAnim = NextAnim::IDLE;
        }

        break;

    }
}

void CPlayerPushState::Exit(CPlayer* pPlayer)
{
    pPlayer->Set_Default();
}

void CPlayerPushState::Hold_Movement(CPlayer* pPlayer)
{
    wstring Key = L"";
	switch (m_ePhase)
	{

	case Client::CPlayerPushState::Phase::Loop:
	{
        if (pPlayerInput->m_bisMove)
            Key = L"lp";

		else
            Key = L"wait";


	  pPlayer->Reserve_Animation_To_Body(L"push_" + Key, true);
    }
	break;

	default:
		break;
	}
    
}

void CPlayerPushState::ChangePhase(CPlayer* pPlayer)
{
    switch (m_ePhase)
    {
    case CPlayerPushState::Phase::Start:
        pPlayer->Reserve_Animation_To_Body(L"push_lp" , true);
        m_ePhase = Phase::Loop;
        break;

    }
}

void CPlayerPushState::ChangeState(CPlayer* pPlayer)
{
    switch (m_eNextAnim)
    {

    case Client::CPlayerPushState::NextAnim::IDLE:
        pPlayer->Change_State(ENUM_TO_UINT(CPlayer::PLAYER_STATE::IDLE));
        break;
    default:
        break;
    }
}

CPlayerPushState* CPlayerPushState::Create()
{
    return new CPlayerPushState;
}

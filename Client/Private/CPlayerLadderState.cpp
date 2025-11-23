#include "CPlayerLadderState.h"
#include "CPlayer.h"

USING(Client)
CPlayerLadderState::CPlayerLadderState()
{
}

CPlayerLadderState::~CPlayerLadderState()
{
}

void CPlayerLadderState::Enter(CPlayer* pPlayer)
{
    pPlayerInput = pPlayer->Get_Input();
    pActionControl = pPlayer->Get_ActionControl();

    pPlayer->Reserve_Animation_To_Body(L"ladder_wait", true);

    m_ePhase = Phase::Loop;
    m_eNextAnim = NextAnim::NONE;

    pPlayer->Set_HideWeapons();


    m_bChange = false;
    m_bChangeState = false;
}

void CPlayerLadderState::Update(CPlayer* pPlayer, _float fTimeDelta)
{
    Hold_Movement(pPlayer);

    if (m_bChange)
        ChangePhase(pPlayer);

    if(m_bChangeState)
        ChangeState(pPlayer);
}

void CPlayerLadderState::Update_Late(CPlayer* pPlayer, _float fTimeDelta)
{
    //페이즈 바꾸는 타이밍
    switch (m_ePhase)
    {

    case Phase::Loop:
        if (!pActionControl->m_bLadder)
        {
            m_bChangeState = true;
            m_eNextAnim = NextAnim::IDLE;

        }
        break;
    }
}

void CPlayerLadderState::Exit(CPlayer* pPlayer)
{
    pPlayer->Set_Default();
}

void CPlayerLadderState::Hold_Movement(CPlayer* pPlayer)
{
    //방향에 따른 애니메이션 적용
//방향에 따른 애니메이션 적용
    if (pPlayer->Get_Transform())
    {
        CTransform* pTransform = pPlayer->Get_Transform();

        wstring dir = L"";
        _float3     fDir;
        _vector MoveDir = pTransform->Get_MoveDir();

        XMStoreFloat3(&fDir, MoveDir);
     
        switch (m_ePhase)
        {

        case Client::CPlayerLadderState::Phase::Loop:
        {
            if (pPlayerInput->m_bisMove)
            {
                if (fDir.y < 0)
                    dir = L"up";
                else
                    dir = L"down";
            }

            else
                dir = L"wait";
        }

        pPlayer->Reserve_Animation_To_Body(L"ladder_" + dir, true);
        break;

        default:
            break;
        }
    }
}

void CPlayerLadderState::ChangePhase(CPlayer* pPlayer)
{
   

    m_bChange = false;
}

void CPlayerLadderState::ChangeState(CPlayer* pPlayer)
{
    switch (m_eNextAnim)
    {
    case NextAnim::IDLE:
        pPlayer->Change_State(ENUM_TO_UINT(CPlayer::PLAYER_STATE::IDLE));
        break;

    }

    m_bChangeState = false;

}

CPlayerLadderState* CPlayerLadderState::Create()
{
    return new CPlayerLadderState;
}                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          

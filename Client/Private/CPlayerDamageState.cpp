#include "CPlayerDamageState.h"
#include "CPlayer.h"

USING(Client)
CPlayerDamageState::CPlayerDamageState()
{
}

CPlayerDamageState::~CPlayerDamageState()
{
}

void CPlayerDamageState::Enter(CPlayer* pPlayer)
{
    pPlayerInput = pPlayer->Get_Input();
    pActionControl = pPlayer->Get_ActionControl();

    //특정 스테이트들 취소

    pActionControl->m_bPush = false;

    //맞은방향 판별한거 가져오기
    if(pPlayer->Get_IsFront())
        pPlayer->Reserve_Animation_To_Body(L"dmg_f", false,true);

    else
        pPlayer->Reserve_Animation_To_Body(L"dmg_b", false, true);
             
    //Damage애니메이션 조금진행됐을때 깜빡거림 시작..

}

bool CPlayerDamageState::Update(CPlayer* pPlayer, _float fTimeDelta)
{
    if (pPlayer->Is_AnimEnd())
    {
        //렌더 꺼주고..
        pPlayer->Set_DamageRender(0.f);
        pPlayer->Change_State(ENUM_TO_UINT(CPlayer::PLAYER_STATE::IDLE));
        
    }

    return true;
}

void CPlayerDamageState::Update_Late(CPlayer* pPlayer, _float fTimeDelta)
{
}

void CPlayerDamageState::Exit(CPlayer* pPlayer)
{
}

CPlayerDamageState* CPlayerDamageState::Create()
{
    return new CPlayerDamageState;
}

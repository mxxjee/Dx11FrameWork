#include "CMonsterIdleState.h"
#include "CMonster.h"


USING(Client)
CMonsterIdleState::CMonsterIdleState()
{
}
CMonsterIdleState::~CMonsterIdleState()
{
}

void CMonsterIdleState::Enter(CMonster* pMonster)
{
    __super::Enter(pMonster);

    pMonster->Reserve_Animation_To_Body(pMonster->Get_AnimKey(CMonster::MONSTER_BASE_STATE::IDLE),true);


}

void CMonsterIdleState::Update(CMonster* pMonster, _float fTimeDelta)
{
    pMonster->UpdateOnIdleState();

    __super::Update(pMonster, fTimeDelta);

    if (m_pActionControl->m_bAttack)
        pMonster->Change_State(ENUM_TO_UINT(CMonster::MONSTER_BASE_STATE::ATTACK));

}

void CMonsterIdleState::Update_Late(CMonster* pMonster, _float fTimeDelta)
{
}

void CMonsterIdleState::Exit(CMonster* pMonster)
{
}


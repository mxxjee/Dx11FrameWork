#include "CMonsterAttackState.h"
#include "CMonster.h"
USING(Client)

CMonsterAttackState::CMonsterAttackState()
{
}

CMonsterAttackState::~CMonsterAttackState()
{
}

void CMonsterAttackState::Enter(CMonster* pMonster)
{
	__super::Enter(pMonster);

	pMonster->Reserve_Animation_To_Body(pMonster->Get_AnimKey(ENUM_TO_UINT(CMonster::MONSTER_BASE_STATE::ATTACK)), true);

}

void CMonsterAttackState::Update(CMonster* pMonster, _float fTimeDelta)
{

	if (!m_pActionControl->m_bAttack)
		pMonster->Change_State(CMonster::RUN);

}

void CMonsterAttackState::Update_Late(CMonster* pMonster, _float fTimeDelta)
{
}

void CMonsterAttackState::Exit(CMonster* pMonster)
{
}

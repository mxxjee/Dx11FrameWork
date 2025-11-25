#include "CMonsterDeadState.h"
#include "CMonster.h"

USING(Client)
CMonsterDeadState::CMonsterDeadState()
{
}

CMonsterDeadState::~CMonsterDeadState()
{
}

void CMonsterDeadState::Enter(CMonster* pMonster)
{
	__super::Enter(pMonster);

	pMonster->Reserve_Animation_To_Body(pMonster->Get_AnimKey(CMonster::MONSTER_BASE_STATE::DIE), false);

}

void CMonsterDeadState::Update(CMonster* pMonster, _float fTimeDelta)
{
	if (pMonster->Is_AnimEnd())
		pMonster->Set_Dead();

}

void CMonsterDeadState::Update_Late(CMonster* pMonster, _float fTimeDelta)
{
}

void CMonsterDeadState::Exit(CMonster* pMonster)
{
}

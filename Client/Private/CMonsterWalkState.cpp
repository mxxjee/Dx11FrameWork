#include "CMonsterWalkState.h"
#include "CMonster.h"


USING(Client)
CMonsterWalkState::CMonsterWalkState()
{
}

CMonsterWalkState::~CMonsterWalkState()
{
}

void CMonsterWalkState::Enter(CMonster* pMonster)
{
	__super::Enter(pMonster);

	pMonster->Reserve_Animation_To_Body(pMonster->Get_AnimKey(CMonster::MONSTER_BASE_STATE::WALK), true);



}

void CMonsterWalkState::Update(CMonster* pMonster, _float fTimeDelta)
{

	__super::Update(pMonster, fTimeDelta);

}

void CMonsterWalkState::Update_Late(CMonster* pMonster, _float fTimeDelta)
{
}

void CMonsterWalkState::Exit(CMonster* pMonster)
{

}

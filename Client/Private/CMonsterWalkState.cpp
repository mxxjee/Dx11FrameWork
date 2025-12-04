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

	pMonster->Reserve_Animation_To_Body(pMonster->Get_AnimKey(ENUM_TO_UINT(CMonster::MONSTER_BASE_STATE::WALK)), true);



}

void CMonsterWalkState::Update(CMonster* pMonster, _float fTimeDelta)
{

	__super::Update(pMonster, fTimeDelta);
	if (!m_pActionControl->m_bMove)
		pMonster->Change_State(CMonster::MONSTER_BASE_STATE::IDLE);

}

void CMonsterWalkState::Update_Late(CMonster* pMonster, _float fTimeDelta)
{
}

void CMonsterWalkState::Exit(CMonster* pMonster)
{

}

void CMonsterWalkState::Check_Always(CMonster* pMonster, _float fTimeDelta)
{
	__super::Check_Always(pMonster, fTimeDelta);

	CheckTrue(pMonster->Is_Dead());



}

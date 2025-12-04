#include "CMMoriblinAttackState.h"
#include "CMonster.h"
#include "CM_MoriblinSword.h"


USING(Client)
CMMoriblinAttackState::CMMoriblinAttackState()
{
}

CMMoriblinAttackState::~CMMoriblinAttackState()
{
	
}

void CMMoriblinAttackState::Enter(CMonster* pMonster)
{
	__super::Enter(pMonster);


	m_pMoriblinSword = dynamic_cast<CM_MoriblinSword*>(pMonster);

	pMonster->Reserve_Animation_To_Body(pMonster->Get_AnimKey(ENUM_TO_UINT(CMonster::MONSTER_BASE_STATE::ATTACK)), true);

}

void CMMoriblinAttackState::Update(CMonster* pMonster, _float fTimeDelta)
{
	__super::Update(pMonster, fTimeDelta);

	
	if (!m_pActionControl->m_bAttack)
		pMonster->Change_State(CMonster::RUN);
}

void CMMoriblinAttackState::Update_Late(CMonster* pMonster, _float fTimeDelta)
{
}

void CMMoriblinAttackState::Exit(CMonster* pMonster)
{
}

void CMMoriblinAttackState::Check_Always(CMonster* pMonster, _float fTimeDelta)
{

	__super::Check_Always(pMonster, fTimeDelta);

	CheckTrue(pMonster->Is_Dead());
	CheckTrue(m_pActionControl->m_bDamage == 1.f);

	if (m_pMoriblinSword->Get_Guard())
		pMonster->Change_State(CM_MoriblinSword::GUARD);


	


}

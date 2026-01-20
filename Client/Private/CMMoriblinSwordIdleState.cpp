#include "CMMoriblinSwordIdleState.h"
#include "CMonster.h"
#include "CM_MoriblinSword.h"


USING(Client)
CMMoriblinSwordIdleState::CMMoriblinSwordIdleState()
{
}

CMMoriblinSwordIdleState::~CMMoriblinSwordIdleState()
{
}

void CMMoriblinSwordIdleState::Enter(CMonster* pMonster)
{
	__super::Enter(pMonster);

	m_pMoriblinSword = dynamic_cast<CM_MoriblinSword*>(pMonster);

	pMonster->Reserve_Animation_To_Body(pMonster->Get_AnimKey(
		ENUM_TO_UINT(CMonster::MONSTER_BASE_STATE::IDLE)), true);

}

void CMMoriblinSwordIdleState::Update(CMonster* pMonster, _float fTimeDelta)
{              
	Check_Always(pMonster, fTimeDelta);

}

void CMMoriblinSwordIdleState::Update_Late(CMonster* pMonster, _float fTimeDelta)
{
}

void CMMoriblinSwordIdleState::Exit(CMonster* pMonster)
{
}

void CMMoriblinSwordIdleState::Check_Always(CMonster* pMonster, _float fTimeDelta)
{
	

	__super::Check_Always(pMonster, fTimeDelta);


	CheckTrue(pMonster->Is_Dead());
	CheckTrue(m_pActionControl->m_bDamage == 1.f);

	if (m_pMoriblinSword->Get_Guard())
	{
		pMonster->Change_State(CM_MoriblinSword::GUARD);
		return;
	}

	if (m_pActionControl->m_bMove)
	{
		pMonster->Change_State(ENUM_TO_UINT(CMonster::MONSTER_BASE_STATE::WALK));
		return;
	}

	if (m_pMoriblinSword->Get_Detect())
	{
		pMonster->Change_State(ENUM_TO_UINT(CM_MoriblinSword::MoriblinState::DETECT));
		return;
	}


}

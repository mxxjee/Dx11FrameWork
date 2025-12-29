#include "CMonsterDamageState.h"
#include "CMonster.h"

USING(Client)
CMonsterDamageState::CMonsterDamageState()
{
}

CMonsterDamageState::~CMonsterDamageState()
{
}

void CMonsterDamageState::Enter(CMonster* pMonster)
{
	__super::Enter(pMonster);

	if(m_eExitType==EXIT_TYPE::ANIM_END)
		pMonster->Reserve_Animation_To_Body(pMonster->Get_AnimKey(ENUM_TO_UINT(CMonster::MONSTER_BASE_STATE::DAMAGE)), false,true);

	else
		pMonster->Reserve_Animation_To_Body(pMonster->Get_AnimKey(ENUM_TO_UINT(CMonster::MONSTER_BASE_STATE::DAMAGE)), true, true);


}

void CMonsterDamageState::Update(CMonster* pMonster, _float fTimeDelta)
{
	if (pMonster->Is_Dead())
	{
		m_pActionControl->m_bDamage = 0.f;
		pMonster->Change_State(ENUM_TO_UINT(CMonster::MONSTER_BASE_STATE::DIE));

	}

	switch (m_eExitType)
	{
	case Client::CMonsterDamageState::EXIT_TYPE::ANIM_END:
		if (pMonster->Is_AnimEnd())
		{
			m_pActionControl->m_bDamage = 0.f;
			pMonster->Change_State(ENUM_TO_UINT(CMonster::MONSTER_BASE_STATE::IDLE));


		}
		break;
	case Client::CMonsterDamageState::EXIT_TYPE::TIMER:
		m_fTime += fTimeDelta;
		if (m_fTime>= m_fExitTime)
		{
			m_pActionControl->m_bDamage = 0.f;
			pMonster->Change_State(ENUM_TO_UINT(CMonster::MONSTER_BASE_STATE::IDLE));


		}

		break;
	}
	
}

void CMonsterDamageState::Update_Late(CMonster* pMonster, _float fTimeDelta)
{

}

void CMonsterDamageState::Exit(CMonster* pMonster)
{
	m_pActionControl->m_bDamage = 0.f;
	m_fTime = 0.f;
}

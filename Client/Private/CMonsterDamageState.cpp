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

	pMonster->Reserve_Animation_To_Body(pMonster->Get_AnimKey(ENUM_TO_UINT(CMonster::MONSTER_BASE_STATE::DAMAGE)), false);


}

void CMonsterDamageState::Update(CMonster* pMonster, _float fTimeDelta)
{
	if (pMonster->Is_AnimEnd())
	{
		m_pActionControl->m_bDamage = false;
		pMonster->Change_State(ENUM_TO_UINT(CMonster::MONSTER_BASE_STATE::IDLE));


	}
}

void CMonsterDamageState::Update_Late(CMonster* pMonster, _float fTimeDelta)
{
}

void CMonsterDamageState::Exit(CMonster* pMonster)
{
}

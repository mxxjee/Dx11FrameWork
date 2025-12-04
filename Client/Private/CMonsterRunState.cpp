#include "CMonsterRunState.h"
#include "CMonster.h"

USING(Client)
CMonsterRunState::CMonsterRunState()
{
}

CMonsterRunState::~CMonsterRunState()
{
}

void CMonsterRunState::Enter(CMonster* pMonster)
{
	__super::Enter(pMonster);

	pMonster->Reserve_Animation_To_Body(pMonster->Get_AnimKey(ENUM_TO_UINT(CMonster::MONSTER_BASE_STATE::RUN)), true);

}

void CMonsterRunState::Update(CMonster* pMonster, _float fTimeDelta)
{
	__super::Update(pMonster, fTimeDelta);

	if(!m_pActionControl->m_bChase)
		pMonster->Change_State(CMonster::IDLE);

	if (m_pActionControl->m_bRange && m_pActionControl->m_bAttack)
		pMonster->Change_State(CMonster::ATTACK);



}

void CMonsterRunState::Update_Late(CMonster* pMonster, _float fTimeDelta)
{
}

void CMonsterRunState::Exit(CMonster* pMonster)
{
}

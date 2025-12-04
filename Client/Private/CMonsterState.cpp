#include "CMonsterState.h"
#include "CGameInstance.h"
#include "CMonster.h"


USING(Client)

CMonsterState::CMonsterState()
	:m_pGameInstance(CGameInstance::GetInstance())
{
}

void CMonsterState::Enter(CMonster* pMonster)
{
	m_pActionControl = pMonster->Get_ActionControl();

}

void CMonsterState::Update(CMonster* pMonster, _float fTimeDelta)
{

	Check_Always(pMonster, fTimeDelta);

}

void CMonsterState::Free()
{
	__super::Free();

}

void CMonsterState::Check_Always(CMonster* pMonster, _float fTimeDelta)
{

	if (m_pActionControl->m_bDead)
		pMonster->Change_State(ENUM_TO_UINT(CMonster::MONSTER_BASE_STATE::DIE));


	CheckTrue(pMonster->Is_Dead());

	if (m_pActionControl->m_bDamage==1.f)
		pMonster->Change_State(ENUM_TO_UINT(CMonster::MONSTER_BASE_STATE::DAMAGE));



}

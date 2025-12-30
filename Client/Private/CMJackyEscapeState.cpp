#include "CMJackyEscapeState.h"
#include "CM_Jacky.h"

USING(Client)

CMJackyEscapeState::CMJackyEscapeState()
{
}

CMJackyEscapeState::~CMJackyEscapeState()
{
}

void CMJackyEscapeState::Enter(CMonster* pMonster)
{
	__super::Enter(pMonster);
	m_pJacky = dynamic_cast<CM_Jacky*>(pMonster);
	CheckNull(m_pJacky);

	//들기 시작
	pMonster->Reserve_Animation_To_Body(pMonster->Get_AnimKey(
		ENUM_TO_UINT(CM_Jacky::JackyState::ESCAPE)), false);


}

void CMJackyEscapeState::Update(CMonster* pMonster, _float fTimeDelta)
{
	if (m_pActionControl->m_bDead)
	{
		pMonster->Change_State(ENUM_TO_UINT(CMonster::MONSTER_BASE_STATE::DIE));
		return;
	}


	CheckTrue(pMonster->Is_Dead());

	if (m_pActionControl->m_bDamage == 1.f)
	{
		pMonster->Change_State(ENUM_TO_UINT(CMonster::MONSTER_BASE_STATE::DAMAGE));
		return;

	}

	if (!m_pJacky->Get_JackyActionInput()->bEscape)
	{
		pMonster->Change_State(ENUM_TO_UINT(CMonster::WALK));
		return;
	}
}

void CMJackyEscapeState::Update_Late(CMonster* pMonster, _float fTimeDelta)
{
}

void CMJackyEscapeState::Exit(CMonster* pMonster)
{
}

void CMJackyEscapeState::Check_Always(CMonster* pMonster, _float fTimeDelta)
{
	__super::Check_Always(pMonster, fTimeDelta);



}

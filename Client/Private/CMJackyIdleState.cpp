#include "CMJackyIdleState.h"
#include "CM_Jacky.h"


USING(Client)

CMJackyIdleState::CMJackyIdleState()
{
}

CMJackyIdleState::~CMJackyIdleState()
{
}

void CMJackyIdleState::Enter(CMonster* pMonster)
{
	__super::Enter(pMonster);
	m_pJacky = dynamic_cast<CM_Jacky*>(pMonster);

	CheckNull(m_pJacky);
	if(!m_pJacky->Is_Carrying())
		pMonster->Reserve_Animation_To_Body(pMonster->Get_AnimKey(
			ENUM_TO_UINT(CMonster::MONSTER_BASE_STATE::IDLE)), true);

}

void CMJackyIdleState::Update(CMonster* pMonster, _float fTimeDelta)
{
	Check_Always(pMonster, fTimeDelta);


	if (m_pActionControl->m_bMove)
		pMonster->Change_State(ENUM_TO_UINT(CMonster::MONSTER_BASE_STATE::WALK));



}

void CMJackyIdleState::Update_Late(CMonster* pMonster, _float fTimeDelta)
{
}

void CMJackyIdleState::Exit(CMonster* pMonster)
{
}

void CMJackyIdleState::Check_Always(CMonster* pMonster, _float fTimeDelta)
{
	// damage받았는지 체크 
	return __super::Check_Always(pMonster, fTimeDelta);

	if(m_pJacky->Get_JackyActionInput()->bLift)
		pMonster->Change_State(ENUM_TO_UINT(CM_Jacky::JackyState::LIFTING));

	if (m_pJacky->Get_JackyActionInput()->bEscape)
		pMonster->Change_State(ENUM_TO_UINT(CM_Jacky::JackyState::ESCAPE));

}

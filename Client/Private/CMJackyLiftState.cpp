#include "CMJackyLiftState.h"
#include "CM_Jacky.h"


USING(Client)

CMJackyLiftState::CMJackyLiftState()
{
}

CMJackyLiftState::~CMJackyLiftState()
{
}

void CMJackyLiftState::Enter(CMonster* pMonster)
{
	__super::Enter(pMonster);
	m_pJacky = dynamic_cast<CM_Jacky*>(pMonster);
	CheckNull(m_pJacky);

	//들기 시작
	pMonster->Reserve_Animation_To_Body(pMonster->Get_AnimKey(
		ENUM_TO_UINT(CM_Jacky::JackyState::LIFTING)), false);


}

void CMJackyLiftState::Update(CMonster* pMonster, _float fTimeDelta)
{
	if (pMonster->Is_AnimEnd())
	{
		//그럼다시 move로복귀
		if(pMonster->Get_ActionControl()->m_bMove)
			pMonster->Change_State(ENUM_TO_UINT(CMonster::MONSTER_BASE_STATE::WALK));

	}
}

void CMJackyLiftState::Update_Late(CMonster* pMonster, _float fTimeDelta)
{
}

void CMJackyLiftState::Exit(CMonster* pMonster)
{
}

void CMJackyLiftState::Check_Always(CMonster* pMonster, _float fTimeDelta)
{
}

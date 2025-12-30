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
}

void CMJackyEscapeState::Update_Late(CMonster* pMonster, _float fTimeDelta)
{
}

void CMJackyEscapeState::Exit(CMonster* pMonster)
{
}

void CMJackyEscapeState::Check_Always(CMonster* pMonster, _float fTimeDelta)
{
}

#include "CMJackyThrowState.h"
#include "CM_Jacky.h"

USING(Client)

CMJackyThrowState::CMJackyThrowState()
{
}

CMJackyThrowState::~CMJackyThrowState()
{
}

void CMJackyThrowState::Enter(CMonster* pMonster)
{
	__super::Enter(pMonster);
	m_pJacky = dynamic_cast<CM_Jacky*>(pMonster);

	CheckNull(m_pJacky);
	pMonster->Reserve_Animation_To_Body(pMonster->Get_AnimKey(
		ENUM_TO_UINT(CM_Jacky::JackyState::THROW)), false);

}

void CMJackyThrowState::Update(CMonster* pMonster, _float fTimeDelta)
{
	if (pMonster->Is_AnimEnd())
		pMonster->Change_State(ENUM_TO_UINT(CMonster::MONSTER_BASE_STATE::IDLE));


}

void CMJackyThrowState::Update_Late(CMonster* pMonster, _float fTimeDelta)
{
}

void CMJackyThrowState::Exit(CMonster* pMonster)
{
}

void CMJackyThrowState::Check_Always(CMonster* pMonster, _float fTimeDelta)
{
}

#include "CMGreenZolHideState.h"
#include "CMonster.h"
#include "CM_GreenZol.h"
USING(Client)
CMGreenZolHideState::CMGreenZolHideState()
{
}

CMGreenZolHideState::~CMGreenZolHideState()
{
}

void CMGreenZolHideState::Enter(CMonster* pMonster)
{
	__super::Enter(pMonster);

	m_pGreenZol = dynamic_cast<CM_GreenZol*>(pMonster);

	pMonster->Reserve_Animation_To_Body(L"depop", false);

}

void CMGreenZolHideState::Update(CMonster* pMonster, _float fTimeDelta)
{
	if (pMonster->Is_AnimEnd())
		pMonster->Change_State(ENUM_TO_UINT(CMonster::MONSTER_BASE_STATE::INTRO));
}

void CMGreenZolHideState::Update_Late(CMonster* pMonster, _float fTimeDelta)
{
}

void CMGreenZolHideState::Exit(CMonster* pMonster)
{
	m_pActionControl->m_bIntroEnd = false;
	m_pActionControl->Reset();
}

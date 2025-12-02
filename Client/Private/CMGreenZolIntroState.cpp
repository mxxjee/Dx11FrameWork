#include "CMGreenZolIntroState.h"
#include "CMonster.h"
#include "CM_GreenZol.h"


USING(Client)
CMGreenZolIntroState::CMGreenZolIntroState()
{
}

CMGreenZolIntroState::~CMGreenZolIntroState()
{
}

void CMGreenZolIntroState::Enter(CMonster* pMonster)
{
	__super::Enter(pMonster);

	pMonster->Reserve_Animation_To_Body(L"under_wait", true);
	m_pGreenZol = dynamic_cast<CM_GreenZol*>(pMonster);
	m_eState = CMGreenZolIntroState::State::WAIT;

}

void CMGreenZolIntroState::Update(CMonster* pMonster, _float fTimeDelta)
{
	//전이조건
	// is Range(첫 감지범위)에 들어오면 spawn애니메이션 재생
	
	switch (m_eState)
	{
	case Client::CMGreenZolIntroState::WAIT:
	{	
		if (m_pActionControl->m_bRange)
		{
			pMonster->Reserve_Animation_To_Body(L"spawn", false);
			m_eState = State::SPAWN;
		}
			
	}
		break;
	case Client::CMGreenZolIntroState::SPAWN:
	{
		if (m_pGreenZol->Is_AnimEnd())
		{
			pMonster->Set_CollisionEnable(true);
			pMonster->Change_State(ENUM_TO_UINT(CMonster::MONSTER_BASE_STATE::IDLE));

		}
			

	}
		break;
	default:
		break;
	}
	




}

void CMGreenZolIntroState::Update_Late(CMonster* pMonster, _float fTimeDelta)
{
}

void CMGreenZolIntroState::Exit(CMonster* pMonster)
{

	m_pActionControl->m_bIntroEnd = true;

}

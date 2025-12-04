#include "CMMoriblinSwordGuardState.h"
#include "CMonster.h"
#include "CM_MoriblinSword.h"


USING(Client)
CMMoriblinSwordGuardState::CMMoriblinSwordGuardState()
{
}

CMMoriblinSwordGuardState::~CMMoriblinSwordGuardState()
{
}

void CMMoriblinSwordGuardState::Enter(CMonster* pMonster)
{
	__super::Enter(pMonster);

	m_pMoriblinSword = dynamic_cast<CM_MoriblinSword*>(pMonster);

	wstring AnimKey = pMonster->Get_AnimKey(ENUM_TO_UINT(CM_MoriblinSword::GUARD));
	if (AnimKey == L"")
		m_eType = GuardType::SWORD;

	else
	{
		m_eType = GuardType::SHIELD;
		pMonster->Reserve_Animation_To_Body(AnimKey, false);
	}

	

}

void CMMoriblinSwordGuardState::Update(CMonster* pMonster, _float fTimeDelta)
{
	switch (m_eType)
	{
	case Client::CMMoriblinSwordGuardState::SHIELD:
		if (pMonster->Is_AnimEnd())
		{
			m_pMoriblinSword->Set_Guard(false, L"");
			pMonster->Change_State(CMonster::ATTACK);
		}
		break;
	case Client::CMMoriblinSwordGuardState::SWORD:
		pMonster->Change_State(CMonster::ATTACK);
		break;

	default:
		break;
	}
}

void CMMoriblinSwordGuardState::Update_Late(CMonster* pMonster, _float fTimeDelta)
{
}

void CMMoriblinSwordGuardState::Exit(CMonster* pMonster)
{
}

void CMMoriblinSwordGuardState::Check_Always(CMonster* pMonster, _float fTimeDelta)
{
}

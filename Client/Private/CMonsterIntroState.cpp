#include "CMonsterIntroState.h"
#include "CMonster.h"

USING(Client)
CMonsterIntroState::CMonsterIntroState()
{
}

CMonsterIntroState::~CMonsterIntroState()
{
}

void CMonsterIntroState::Enter(CMonster* pMonster)
{
	__super::Enter(pMonster);

	pMonster->Reserve_Animation_To_Body(pMonster->Get_AnimKey(ENUM_TO_UINT(CMonster::MONSTER_BASE_STATE::INTRO)),false);
	pMonster->Intro_Behavor();

}

void CMonsterIntroState::Update(CMonster* pMonster, _float fTimeDelta)
{
	//전이 조건
	if (pMonster->Is_AnimEnd())
		pMonster->Change_State(ENUM_TO_UINT(CMonster::MONSTER_BASE_STATE::IDLE));

}

void CMonsterIntroState::Update_Late(CMonster* pMonster, _float fTimeDelta)
{
	}

void CMonsterIntroState::Exit(CMonster* pMonster)
{

}

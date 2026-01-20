#include "CMMoriblinSwordDetectState.h"
#include "CMonster.h"
#include "CM_MoriblinSword.h"


USING(Client)
CMMoriblinSwordDetectState::CMMoriblinSwordDetectState()
{
}

CMMoriblinSwordDetectState::~CMMoriblinSwordDetectState()
{
}

void CMMoriblinSwordDetectState::Enter(CMonster* pMonster)
{
	__super::Enter(pMonster);

	pMonster->Reserve_Animation_To_Body(pMonster->Get_AnimKey(
		ENUM_TO_UINT(CM_MoriblinSword::MoriblinState::DETECT)), false);

}

void CMMoriblinSwordDetectState::Update(CMonster* pMonster, _float fTimeDelta)
{
	if (pMonster->Is_AnimEnd())
		pMonster->Change_State(ENUM_TO_UINT(CMonster::RUN));


}

void CMMoriblinSwordDetectState::Update_Late(CMonster* pMonster, _float fTimeDelta)
{
}

void CMMoriblinSwordDetectState::Exit(CMonster* pMonster)
{
}



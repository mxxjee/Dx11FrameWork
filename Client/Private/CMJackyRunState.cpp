#include "CMJackyRunState.h"
#include "CM_Jacky.h"



USING(Client)

CMJackyRunState::CMJackyRunState()
{
}

CMJackyRunState::~CMJackyRunState()
{
}

void CMJackyRunState::Enter(CMonster* pMonster)
{
	__super::Enter(pMonster);
	m_pJacky = dynamic_cast<CM_Jacky*>(pMonster);

	CheckNull(m_pJacky);

	//들고있는게있는지 없는지 판단하여 애니메이션을 다르게 재생한다.


	if(m_pJacky->Is_Carrying())//상호작용중이라면.
		pMonster->Reserve_Animation_To_Body(pMonster->Get_AnimKey(
			ENUM_TO_UINT(CM_Jacky::JackyState::LIFTING_MOVE)), true);

	else
		pMonster->Reserve_Animation_To_Body(pMonster->Get_AnimKey(
			ENUM_TO_UINT(CMonster::MONSTER_BASE_STATE::RUN)), false);

	m_fTime = 0.f;

}

void CMJackyRunState::Update(CMonster* pMonster, _float fTimeDelta)
{

	if (m_pJacky->Is_Carrying())
	{
		//상호작용중이라면, 던질수잇는지 체크
		if(m_pJacky->Get_JackyActionInput()->bThrow)
			pMonster->Change_State(ENUM_TO_UINT(CM_Jacky::JackyState::THROW));

		
		m_fTime += fTimeDelta;
		if (m_fTime >= 0.5f)
			//다시돌아갈수있는지 체크
		{
			pMonster->Change_State(ENUM_TO_UINT(CMonster::MONSTER_BASE_STATE::IDLE));
			m_fTime = 0.f;
		}

	}

	else
	{
	

		//데미지받을 수 있는지 체크
		//죽을수 있는지 체크
		Check_Always(pMonster,fTimeDelta);

		//다시 idle로 돌아갈수잇는지 체크
		if (pMonster->Is_AnimEnd())
			pMonster->Change_State(ENUM_TO_UINT(CMonster::MONSTER_BASE_STATE::IDLE));


		//공잡을 수 있는지 체크


	}
}

void CMJackyRunState::Update_Late(CMonster* pMonster, _float fTimeDelta)
{
}

void CMJackyRunState::Exit(CMonster* pMonster)
{
}

void CMJackyRunState::Check_Always(CMonster* pMonster, _float fTimeDelta)
{
	__super::Check_Always(pMonster, fTimeDelta);

	if (m_pJacky->Get_JackyActionInput()->bLift)
		pMonster->Change_State(ENUM_TO_UINT(CM_Jacky::JackyState::LIFTING));

	if (m_pJacky->Get_JackyActionInput()->bEscape)
		pMonster->Change_State(ENUM_TO_UINT(CM_Jacky::JackyState::ESCAPE));

}

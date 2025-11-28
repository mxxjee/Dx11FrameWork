#include "CMGreenZolJumpAttackState.h"
#include "CMonster.h"
#include "CM_GreenZol.h"

USING(Client)

CMGreenZolJumpAttackState::CMGreenZolJumpAttackState()
{
}

CMGreenZolJumpAttackState::~CMGreenZolJumpAttackState()
{
}

void CMGreenZolJumpAttackState::Enter(CMonster* pMonster)
{
	__super::Enter(pMonster);
	
	m_pGreenZol = dynamic_cast<CM_GreenZol*>(pMonster);

	pMonster->Reserve_Animation_To_Body(L"jump_sign", true);
	m_ePhase = PHASE::SIGN;
	m_fTime = 0.f;

	pMonster->Set_CanMove(false);
}

void CMGreenZolJumpAttackState::Update(CMonster* pMonster, _float fTimeDelta)
{
	
	switch (m_ePhase)
	{
	case Client::CMGreenZolJumpAttackState::SIGN:
	{
		/// <summary>
		/// 전이조건 :시간
	
		if (m_pGreenZol->Get_IsHide())
			pMonster->Change_State(ENUM_TO_UINT(CMonster::MONSTER_BASE_STATE::IDLE));

		else
		{
			m_fTime += fTimeDelta;
			if (m_fTime >= 1.f)
			{
				m_fTime = 0.f;
				
				m_pGreenZol->JumpStart_Behavior();
				m_ePhase = PHASE::START;
				pMonster->Reserve_Animation_To_Body(L"jump_st", false);
				pMonster->Set_CanMove(false);
			}
		}
		
	}
		break;



	case Client::CMGreenZolJumpAttackState::START:
		m_pGreenZol->JumpLoop_Behavior(fTimeDelta);
		if (pMonster->Is_AnimEnd())
		{
			
			m_ePhase = PHASE::LOOP;
			m_pGreenZol->JumpEnd_Behavior();
			pMonster->Reserve_Animation_To_Body(L"jump_loop", true);
			pMonster->Set_CanMove(true);
		}
		break;



	case Client::CMGreenZolJumpAttackState::LOOP:
		if (m_pGreenZol->Get_IsOnGround())
		{

			m_ePhase = PHASE::ED;
			pMonster->Reserve_Animation_To_Body(L"jump_ed", false);
			pMonster->Set_CanMove(false);
		}
		break;

	case Client::CMGreenZolJumpAttackState::ED:
		if (pMonster->Is_AnimEnd())
			pMonster->Change_State(ENUM_TO_UINT(CMonster::MONSTER_BASE_STATE::IDLE));


		break;
	case Client::CMGreenZolJumpAttackState::END:
		break;
	default:
		break;
	}
}

void CMGreenZolJumpAttackState::Update_Late(CMonster* pMonster, _float fTimeDelta)
{

}

void CMGreenZolJumpAttackState::Exit(CMonster* pMonster)
{
	m_pActionControl->m_bAttack = false;
	pMonster->Set_CanMove(true);
}

#include "CM_GreenZol.h"
#include "CMonster_Body.h"
#include "CPlayer.h"


//////////////States///////////
#include "MonsterStates.h"
#include "CMGreenZolIntroState.h"
#include "CMGreenZolJumpAttackState.h"
#include "CMGreenZolHideState.h"




USING(Client)

CM_GreenZol::CM_GreenZol(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CMonster(pDevice,pContext)
{
}

CM_GreenZol::CM_GreenZol(const CM_GreenZol& rhs)
	:CMonster(rhs)
{
}

HRESULT CM_GreenZol::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;


	return S_OK;
}

HRESULT CM_GreenZol::Initialize_Copytype(void* pArg)
{
	if (FAILED(__super::Initialize_Copytype(pArg)))
		return E_FAIL;

	Register_Anim();


	

	if (FAILED(Ready_States()))
		return E_FAIL;



	Change_State(ENUM_TO_UINT(CMonster::MONSTER_BASE_STATE::INTRO));
	m_eCurState = MONSTER_BASE_STATE::INTRO;

	return S_OK; 
}

void CM_GreenZol::Update_Priority(_float fTimeDelta)
{
	__super::Update_Priority(fTimeDelta);
}

void CM_GreenZol::Update(_float fTimeDelta)
{
	Intro_Behavor();
	AIState_Change(fTimeDelta);

	if (m_pNextState != nullptr)
	{
		m_pCurState = m_pNextState;
		m_pNextState = nullptr;

	}


	if (m_pCurState)
		m_pCurState->Update(this, fTimeDelta);

	__super::Update(fTimeDelta);
}

void CM_GreenZol::Update_Late(_float fTimeDelta)
{
	__super::Update_Late(fTimeDelta);

	Update_Movement(fTimeDelta);
}

void CM_GreenZol::Update_Render(_float fTimeDelta)
{
	__super::Update_Render(fTimeDelta);
}

HRESULT CM_GreenZol::Render()
{
	__super::Render();



	return S_OK;
}

void CM_GreenZol::Register_Anim()
{
	if (m_pMonsterBody)
	{

		m_pMonsterBody->Register_AnimKey(CMonster::MONSTER_BASE_STATE::INTRO, L"under_wait");
		m_pMonsterBody->Register_AnimKey(CMonster::MONSTER_BASE_STATE::IDLE, L"wait");
		m_pMonsterBody->Register_AnimKey(CMonster::MONSTER_BASE_STATE::WALK, L"walk");
		m_pMonsterBody->Register_AnimKey(CMonster::MONSTER_BASE_STATE::JUMP, L"jump_st");
		m_pMonsterBody->Register_AnimKey(CMonster::MONSTER_BASE_STATE::DIE, L"dead");



		//////////Speed///////
		m_pMonsterBody->Set_Animation_Speed(L"spawn", 25.f);
		m_pMonsterBody->Set_Animation_Speed(L"jump_sign",50.f);
		m_pMonsterBody->Set_Animation_Speed(L"depop", 50.f);

	}



}

void CM_GreenZol::Intro_Behavor()
{
	CheckTrue(m_ActionControl.m_bIntroEnd);

	m_ActionControl.m_bRange = Is_InRange(m_fSpawnRange);
	if (m_ActionControl.m_bRange)
		m_bHide = false;


}

void CM_GreenZol::Change_State(int newState)
{
	if (m_pCurState == m_States[newState])
		return;

	if (m_pCurState && !m_pCurState->CanExit())
		return;

	if (m_pCurState)
		m_pCurState->Exit(this);

	m_iPreState = m_iState;
	m_iState = newState;


	m_pNextState = m_States[newState];
	m_pNextState->Enter(this);

	//시간초기화..
	m_eCurState = static_cast<MONSTER_BASE_STATE>(newState);
	m_fTime = 0.f;
	

}

void CM_GreenZol::AIState_Change(_float fTimeDelta)
{
	CheckFalse(m_ActionControl.m_bIntroEnd);
	//인트로 시작안했으면 이후처리X

	if (iHp <= 0)
		m_ActionControl.m_bDead = true;


	CheckTrue(m_ActionControl.m_bDead);

	switch (m_eCurState)
	{
	case Client::CMonster::MONSTER_BASE_STATE::IDLE:
		//알아서 wakl로 갈수이또록 기반작업
		Idle_Behavior(fTimeDelta);
		break;

	case Client::CMonster::MONSTER_BASE_STATE::WALK:
		break;
	case Client::CMonster::MONSTER_BASE_STATE::RUN:
		break;
	case Client::CMonster::MONSTER_BASE_STATE::JUMP:
		break;
	case Client::CMonster::MONSTER_BASE_STATE::ATTACK:

		break;
	case Client::CMonster::MONSTER_BASE_STATE::DAMAGE:
		break;
	case Client::CMonster::MONSTER_BASE_STATE::DIE:
		break;
	case Client::CMonster::MONSTER_BASE_STATE::END:
		break;
	default:
		break;
	}
}

void CM_GreenZol::Update_Movement(_float fTimeDelta)
{
	switch (m_eCurState)
	{
	
		//랜덤한 방향으로 이동
	case Client::CMonster::MONSTER_BASE_STATE::WALK:
		Move_RandomDir();
		break;

		//플레이어 쳐다보고 플레이어방향으로 점프
	case Client::CMonster::MONSTER_BASE_STATE::ATTACK:
		Jump_To_Player(fTimeDelta);
		
		break;


	default:
		break;
	}
}

void CM_GreenZol::Render_StateDebug(int* pArg)
{
	if (ImGui::RadioButton("Die", (int*)(pArg), 0))
	{
		iHp = 0;

	}
	
}

void CM_GreenZol::Move_RandomDir()
{

	int RandomValue = rand() % 2;		//+ or - 
	int RandomDir = rand() % 3;		//zero, forward,right

	
	switch (RandomValue)
	{
	case 0:
		break;

	default:
		break;
	}

	m_pTransformCom->Set_Speed(m_fInitSpeed);
	//m_pTransformCom->Move()
}

void CM_GreenZol::UpdateOnIdleState()
{
	if (m_bHide)
		Change_State(ENUM_TO_UINT(CM_GreenZol::GreenZolState::HIDE));

}

void CM_GreenZol::Jump_To_Player(_float fTimeDelta)
{
	CGameObject* pPlayer = m_pGameInstance->Find_GameObject(0, L"Player_Layer", L"Player");
	CTransform* pPlayerTrans = pPlayer->Get_Transform();

	if (pPlayerTrans)
	{
		_vector PlayerPos = pPlayerTrans->Get_State(STATE::POSITION, TransformScope::WORLD);
		m_pTransformCom->LookAtSmooth(PlayerPos, 3.f, fTimeDelta);
		CheckFalse(m_bCanMove);

	}


	
}



CM_GreenZol* CM_GreenZol::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
{
	CM_GreenZol* pInstance = new CM_GreenZol(_pDevice, _pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create :CM_GreenZol ");
		Safe_Release(pInstance);

	}



	return pInstance;
}

CGameObject* CM_GreenZol::Clone(void* pArg)
{
	CM_GreenZol* pInstance = new CM_GreenZol(*this);
	if (FAILED(pInstance->Initialize_Copytype(pArg)))
	{
		MSG_BOX("Failed to Cloned :CM_GreenZol ");
		Safe_Release(pInstance);

	}
	return pInstance;
}

void CM_GreenZol::Free()
{
	__super::Free();
}

HRESULT CM_GreenZol::Ready_States()
{
	m_States.emplace(ENUM_TO_UINT(CMonster::MONSTER_BASE_STATE::INTRO), CMGreenZolIntroState::Create());
	m_States.emplace(ENUM_TO_UINT(CMonster::MONSTER_BASE_STATE::IDLE), CMonsterIdleState::Create());
	m_States.emplace(ENUM_TO_UINT(CMonster::MONSTER_BASE_STATE::ATTACK), CMGreenZolJumpAttackState::Create());
	m_States.emplace(ENUM_TO_UINT(CM_GreenZol::GreenZolState::HIDE), CMGreenZolHideState::Create());
	m_States.emplace(ENUM_TO_UINT(CMonster::MONSTER_BASE_STATE::DIE), CMonsterDeadState::Create());




	return S_OK;
}

void CM_GreenZol::Idle_Behavior(_float fTimeDelta)
{
	//대기시간 설정
	m_fTime += fTimeDelta;
	if (m_fTime >= m_fSecond)
	{

		//actionrange라면 aTTACK(jump공격)
		if (Is_InRange(fActionRange))
			m_ActionControl.m_bAttack = true;


		else
		{
			m_bHide = true;
			m_ActionControl.m_bAttack = false;

		}
		m_fTime = 0.f;

	}


}

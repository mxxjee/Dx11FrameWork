#include "CM_GreenZol.h"
#include "CMonster_Body.h"
#include "CPlayer.h"

////////////////////Components/////////
#include "CGravity.h"
#include "CNavigation.h"
#include "CCollider_Base.h"
#include "CBounding_AABB.h"
#include "CBoxColliderComponent.h"
#include "CCell.h"



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

	if (FAILED(Ready_Component(pArg)))
		return E_FAIL;

	

	if (FAILED(Ready_States()))
		return E_FAIL;


	if (m_pNavigationCom)
	{
		m_pTransformCom->Set_State(STATE::POSITION, m_pNavigationCom->Get_Cell(30)->Get_CenterPos());
		m_pNavigationCom->Set_CurrentIdx(m_pTransformCom->Get_State(STATE::POSITION));
	}

	Change_State(ENUM_TO_UINT(CMonster::MONSTER_BASE_STATE::INTRO));
	m_eCurState = MONSTER_BASE_STATE::INTRO;

	m_pCollider->Set_Active(false);

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
	CModelObject::Update_Late(fTimeDelta);

	//밀렸을떄 움직임..
	m_pTransformCom->UpdateImpulse(fTimeDelta, m_pNavigationCom);
	Damage_Behavior(fTimeDelta);

	Update_Movement(fTimeDelta);

	if (m_pCurState)
		m_pCurState->Update_Late(this, fTimeDelta);

	m_pCollider->Update_Collider(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));

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
		m_pMonsterBody->Set_Animation_Speed(L"jump_loop", 50.f);
		m_pMonsterBody->Set_Animation_Speed(L"depop", 50.f);

	}



}

void CM_GreenZol::Intro_Behavor()
{
	CheckTrue(m_ActionControl.m_bIntroEnd);

	m_ActionControl.m_bRange = Is_InRange(m_fSpawnRange);
	if (m_ActionControl.m_bRange)
	{
		m_bHide = false;
		
	}


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



HRESULT CM_GreenZol::Ready_Component(void* pArg)
{
	CComponent::COMPONENT_DESC Desc;
	Desc.pOwner = this;


	/////////////////Gravity추가
	CComponent* pGravity = dynamic_cast<CGravity*>(m_pGameInstance->Clone_Prototype(
		PROTOTYPE::COMPONENT,
		0,
		PROTO_COMPONENT_NAME(L"Gravity"),
		&Desc)
		);

	if (FAILED(Add_Component(
		COMPONENT_TYPE::GRAVITYCOM,
		pGravity,
		reinterpret_cast<CComponent**>(&m_pGravity)
	)))
		return E_FAIL;


	//////////////Boxcollider추가
	CCollider_Base::COLLIDER_DESC pColliderDesc;
	pColliderDesc.m_eColGroup = ENUM_TO_UINT(COLLISION_GROUP::MONSTER);


	CBounding_AABB::BOUNDING_AABB_DESC CollDesc;
	CollDesc.vCenter = { 0.f,0.5f,0.f };
	CollDesc.Extents = { 0.3f,0.8f,0.4f };
	pColliderDesc.m_BoundingDesc = &CollDesc;


	CComponent* pCollider = dynamic_cast<CBoxColliderComponent*>(m_pGameInstance->Clone_Prototype(
		PROTOTYPE::COMPONENT,
		0,
		PROTO_COMPONENT_NAME(L"BoxCollider"),
		&pColliderDesc)
		);

	if (FAILED(Add_Component(
		COMPONENT_TYPE::BOX_COLLIDER,
		pCollider,
		reinterpret_cast<CComponent**>(&m_pCollider)
	)))
		return E_FAIL;

	return S_OK;
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
	if (m_pGravity->IsOnGround())
		m_pTransformCom->Set_State(STATE::POSITION,
			m_pNavigationCom->SetUp_OnNavigation(m_pTransformCom->Get_State(STATE::POSITION)));


	CheckTrue(m_ActionControl.m_bDead);

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

	}

	m_pGravity->Update(fTimeDelta);
	// 이번 프레임 Y 이동량
	float fDT = m_pGravity->GetFallDistance(fTimeDelta);

	_vector vCurPos = m_pTransformCom->Get_State(STATE::POSITION);
	_vector vNewPos = vCurPos + XMVectorSet(0.f, fDT, 0.f, 0.f);

	//바닥 체크
	_float vOutY = 0.f;
	bool bOnGround = m_pNavigationCom->CheckGround(vNewPos, vOutY);

	//------------------------------------------------
	//점프 중 (위로 뜨거나, 아직 공중일 때)
	//------------------------------------------------
	if (m_pGravity->IsJumping())
	{
		//점프 시 위치설정
		m_pTransformCom->Set_State(STATE::POSITION, vNewPos);

		//낙하 상태체크
		if (m_pGravity->GetVelocityY() <= 0.f)
		{
			m_pGravity->SetJumping(false);
		}

		return;
	}

	//------------------------------------------------
	// 점프 중은 아닌데, 아직 공중 (떨어지는 중)
	//------------------------------------------------
	if (!bOnGround)
	{
		m_pTransformCom->Set_State(STATE::POSITION, vNewPos);
		m_pGravity->SetOnGround(false);
		return;
	}

	//------------------------------------------------
	// 바닥 감지 && 떨어지는 중-> 착지 처리
	//------------------------------------------------
	if (m_pGravity->GetVelocityY() <= 0.f)
	{
		// 바닥 높이로 스냅
		vNewPos = XMVectorSetY(vNewPos, vOutY);
		m_pTransformCom->Set_State(STATE::POSITION, vNewPos);

		// 착지
		m_pGravity->Land();      // 여기서 Velocity 0, OnGround=true, Jumping=false
		return;
	}

	//------------------------------------------------
	//  그냥 평지 위에 서 있는 상태
	//------------------------------------------------
	m_pGravity->SetOnGround(true);

	
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
	Safe_Release(m_pGravity);
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

void CM_GreenZol::JumpStart_Behavior()
{
	m_bJump = true;
	m_pGravity->Jump(25.f);

}

void CM_GreenZol::JumpLoop_Behavior(_float fTimeDelta)
{
	CheckFalse(m_bJump);
	m_pTransformCom->Move(DIRECTION::FORWARD, fTimeDelta, Space::Local, m_pNavigationCom);

}

void CM_GreenZol::JumpEnd_Behavior()
{
	m_bJump = false;
}

bool CM_GreenZol::Get_IsOnGround()
{
	return m_pGravity->IsOnGround();
}


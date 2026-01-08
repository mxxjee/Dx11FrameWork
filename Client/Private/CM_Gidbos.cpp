#include "CM_Gidbos.h"
#include "CMonster_Body.h"
#include "CPlayer.h"
#include "CRangeCollider.h"
#include "MonsterStates.h"

////////////////////Components/////////
#include "CGravity.h"
#include "CNavigation.h"
#include "CCollider_Base.h"
#include "CSphereColliderComponent.h"
#include "CBoxColliderComponent.h"
#include "CBounding_AABB.h"


#include "CCell.h"

USING(Client)

CM_Gidbos::CM_Gidbos(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CMonster(pDevice,pContext)
{
}

CM_Gidbos::CM_Gidbos(const CM_Gidbos& rhs)
	: CMonster(rhs)
{
}

void CM_Gidbos::Enter_State(int newState)
{
	__super::Enter_State(newState);

	switch (newState)
	{
	case ENUM_TO_UINT(CMonster::MONSTER_BASE_STATE::IDLE):
	{
		m_pGameInstance->Invoke(0.5f, false, false, false, [this]()
			{
				//대기 후 1초뒤 양옆으로이동
				m_ActionControl.m_bMove = true; 
			},this);
	}
		break;

	case ENUM_TO_UINT(CMonster::MONSTER_BASE_STATE::WALK):
	{
		

	}
	break;
	default:
		break;
	}
}

HRESULT CM_Gidbos::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CM_Gidbos::Initialize_Copytype(void* pArg)
{
	GIDBOS_DESC* pDesc = static_cast<GIDBOS_DESC*>(pArg);

	if (FAILED(__super::Initialize_Copytype(pArg)))
		return E_FAIL;



	if (FAILED(Ready_States()))
		return E_FAIL;

	Register_Anim();




	if (m_pNavigationCom)
	{
		m_pNavigationCom->Set_CurrentIdx(m_pTransformCom->Get_State(STATE::POSITION));
	}


	Change_State(ENUM_TO_UINT(CMonster::MONSTER_BASE_STATE::IDLE));



	m_ActionControl.m_bIntroEnd = true;

	m_vInitPos = m_pTransformCom->Get_State(STATE::POSITION);
	m_vMoveDir = XMVector3Normalize(
		m_pTransformCom->Get_State(STATE::LOOK)
	);

	m_fMoveLength = 6.f;
	
	// 두 끝점

	
	m_vPointA = XMLoadFloat3(&pDesc->m_vPointA);
	m_vPointB = XMLoadFloat3(&pDesc->m_vPointB);

	m_vPointA = XMVectorSetW(m_vPointA, 1.f);
	m_vPointB = XMVectorSetW(m_vPointB, 1.f);


	// 처음 목표는 A
	if (pDesc->m_StartLeft)
	{
		m_vMoveTargetPos = m_vPointB;
		m_pTransformCom->Set_State(STATE::POSITION, m_vPointA);
		

	}

	else
	{
		m_vMoveTargetPos = m_vPointA;
		m_pTransformCom->Set_State(STATE::POSITION, m_vPointB);
		
	}
	m_pNavigationCom->Set_CurrentIdx(m_pTransformCom->Get_State(STATE::POSITION));

	
	return S_OK;
}

void CM_Gidbos::Update_Priority(_float fTimeDelta)
{
	__super::Update_Priority(fTimeDelta);
}

void CM_Gidbos::Update(_float fTimeDelta)
{
	AIState_Change(fTimeDelta);



	if (m_pCurState)
		m_pCurState->Update(this, fTimeDelta);

	__super::Update(fTimeDelta);

}

void CM_Gidbos::Update_Late(_float fTimeDelta)
{
	__super::Update_Late(fTimeDelta);


}

void CM_Gidbos::Update_Render(_float fTimeDelta)
{
	__super::Update_Render(fTimeDelta);
}


HRESULT CM_Gidbos::Render()
{
	__super::Render();



	return S_OK;
}

void CM_Gidbos::Register_Anim()
{
	if (m_pMonsterBody)
	{
		m_pMonsterBody->Register_AnimKey(ENUM_TO_UINT(CMonster::MONSTER_BASE_STATE::IDLE), L"wait");
		m_pMonsterBody->Register_AnimKey(ENUM_TO_UINT(CMonster::MONSTER_BASE_STATE::WALK), L"walk");
		m_pMonsterBody->Register_AnimKey(ENUM_TO_UINT(CMonster::MONSTER_BASE_STATE::DAMAGE), L"stun");
		m_pMonsterBody->Register_AnimKey(ENUM_TO_UINT(CMonster::MONSTER_BASE_STATE::DIE), L"dead_f");



		//////////Speed///////
		m_pMonsterBody->Set_Animation_Speed(L"walk", 50.f);
		m_pMonsterBody->Set_Animation_Speed(L"wait", 50.f);


	}
}

void CM_Gidbos::Idle_Behavior(_float fTimeDelta)
{
	//대기상태
	CheckNull(m_pRadiusTrigger);

	//if (m_pRadiusTrigger->Is_Collision())
	m_ActionControl.m_bMove = true;

	//그냥무지성왔다갔다하기

}

void CM_Gidbos::Walk_Behavior(_float fTimeDelta)
{

	CheckNull(m_pNavigationCom);


	bool m_bChase = m_pTransformCom->Chase(m_vMoveTargetPos, fTimeDelta, m_pNavigationCom, 1.f);
	m_pTransformCom->LookAtSmooth_Quaternion(m_vMoveTargetPos,5.f,fTimeDelta);

	if (m_bChase)
	{
		// 목표 지점 토글
		if (m_fChangeDir == 1.f)
			m_vMoveTargetPos = m_vPointB;
		else
			m_vMoveTargetPos = m_vPointA;

		m_fChangeDir *= -1.f;
	}
	
	//// 이동
	//m_pTransformCom->Move(
	//	DIRECTION::FORWARD,
	//	fTimeDelta,
	//	Space::Local,
	//	m_pNavigationCom
	//);

	////이동 후 위치로 다시 계산
	//_vector vCurPos = m_pTransformCom->Get_State(STATE::POSITION);

	//float fDist = XMVectorGetX(
	//	XMVector3Length(m_vMoveTargetPos - vCurPos)
	//);

	//if (fDist <= 0.2f)
	//{
	//	// 목표 지점 토글
	//	if(m_fChangeDir==1.f)
	//		m_vMoveTargetPos = m_vPointB;
	//	else
	//		m_vMoveTargetPos = m_vPointA;

	//	m_fChangeDir *= -1.f;
	//}
}



HRESULT CM_Gidbos::Ready_PartObjects(void* pArg)
{
	if(FAILED(__super::Ready_PartObjects(pArg)))
		return E_FAIL;


	/// <summary>
	/// /RangeTrigger준비
	
	//CRangeCollider::RANGE_COLLIDER_DESC RangeDesc;
	//RangeDesc.fRadius = 5.f;
	//RangeDesc.m_iLevelID = m_iLevelID;
	//RangeDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();

	//if (FAILED(__super::Add_PartObject(0, PROTO_OBJ_NAME(L"RangeCollider"), L"RangeCollider", &RangeDesc)))
	//	return E_FAIL;

	//m_pRadiusTrigger = dynamic_cast<CRangeCollider*>(Find_PartObject(L"RangeCollider"));


	return S_OK;
}

HRESULT CM_Gidbos::Ready_Components(void* pArg)
{
	if (FAILED(__super::Ready_Components(pArg)))
		return E_FAIL;


	CComponent::COMPONENT_DESC Desc;
	Desc.pOwner = this;

	//////////////Boxcollider추가
	CCollider_Base::COLLIDER_DESC pColliderDesc;
	pColliderDesc.m_eColGroup = ENUM_TO_UINT(COLLISION_GROUP::MONSTER);
	pColliderDesc.m_iLevelID = m_iSceneID;

	CBounding_AABB::BOUNDING_AABB_DESC CollDesc;
	CollDesc.vCenter = { 0.f,0.5f,0.f };
	CollDesc.Extents = { 0.5f,0.5f,0.5f };
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

void CM_Gidbos::AIState_Change(_float fTimeDelta)
{
	CheckFalse(m_ActionControl.m_bIntroEnd);
	//인트로 시작안했으면 이후처리X

	Update_DeadState(fTimeDelta);


	CheckTrue(m_ActionControl.m_bDead);

	switch (m_iState)
	{
	case Client::CMonster::MONSTER_BASE_STATE::IDLE:
		//sphere감지 되면 idle->movest 
		Idle_Behavior(fTimeDelta);
		break;

	case Client::CMonster::MONSTER_BASE_STATE::WALK:
		Walk_Behavior(fTimeDelta);
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
HRESULT CM_Gidbos::Ready_States()
{
	m_States.emplace(CMonster::MONSTER_BASE_STATE::IDLE, CMonsterIdleState::Create());
	m_States.emplace(CMonster::MONSTER_BASE_STATE::WALK, CMonsterWalkState::Create());
	m_States.emplace(CMonster::MONSTER_BASE_STATE::DAMAGE, CMonsterDamageState::Create(CMonsterDamageState::EXIT_TYPE::TIMER));
	m_States.emplace(CMonster::MONSTER_BASE_STATE::DIE, CMonsterDeadState::Create());




	return S_OK;
}

void CM_Gidbos::Update_Movement(_float fTimeDelta)
{
}

void CM_Gidbos::Render_StateDebug(int* pArg)
{
}


CM_Gidbos* CM_Gidbos::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
{
	CM_Gidbos* pInstance = new CM_Gidbos(_pDevice, _pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create :CM_Gidbos:: ");
		Safe_Release(pInstance);

	}



	return pInstance;
}

CGameObject* CM_Gidbos::Clone(void* pArg)
{
	CM_Gidbos* pInstance = new CM_Gidbos(*this);
	if (FAILED(pInstance->Initialize_Copytype(pArg)))
	{
		MSG_BOX("Failed to Cloned :CM_Gidbos");
		Safe_Release(pInstance);

	}
	return pInstance;
}

void CM_Gidbos::Free()
{
	Safe_Release(m_pRadiusTrigger);
	__super::Free();
	


	
}

void CM_Gidbos::OnCollisionEnter(_uint iGroup, CCollider_Base* pOther)
{
	CheckFalse(m_bCanCollision);

	CGameObject* pOwner = pOther->Get_Owner();
	switch (COLLISION_GROUP(iGroup))
	{
	case COLLISION_GROUP::PLAYER_WEAPON:
	{
		m_ActionControl.m_bDamage = 1.f;
		--iHp;

		CPartObject* pPart = dynamic_cast<CPartObject*>(pOwner);
		if (pPart)
		{

			//pOther을 바라보고,
			m_pTransformCom->LookAt(pPart->Get_Owner()->Get_Transform());

			_float3 vDir;
			XMStoreFloat3(&vDir, m_pTransformCom->Get_State(STATE::LOOK));
			m_pTransformCom->AddImpulse(-0.1f, vDir);
		}


		_vector vDir = (pPart->Get_Owner()->Get_Transform()->Get_State(STATE::POSITION))
			- (m_pTransformCom->Get_State(STATE::POSITION));

		bool bHitFront = m_pTransformCom->IsFront(vDir);
		if (bHitFront)
		{
			m_pMonsterBody->Change_AnimKey(ENUM_TO_UINT(CMonster::MONSTER_BASE_STATE::DAMAGE), L"damage_f");
			m_pMonsterBody->Change_AnimKey(ENUM_TO_UINT(CMonster::MONSTER_BASE_STATE::DIE), L"dead_f");
		}
		else
		{
			m_pMonsterBody->Change_AnimKey(ENUM_TO_UINT(CMonster::MONSTER_BASE_STATE::DAMAGE), L"damage_b");
			m_pMonsterBody->Change_AnimKey(ENUM_TO_UINT(CMonster::MONSTER_BASE_STATE::DIE), L"dead_b");


		}

		m_bCanCollision = false;

		m_pGameInstance->Invoke(0.5f, 0.f, false, false, [this]()
			{
				CheckTrue(m_bCanCollision);
				m_bCanCollision = true;
			}, this);

		if (iHp <= 0)
			m_ActionControl.m_bDead = true;

	}
	break;

	}

}
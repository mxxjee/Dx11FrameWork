#include "CM_MoriblinSword.h"
#include "CMonster_Body.h"
#include "CPlayer.h"
#include "CMMoriblin_Weapon.h"
#include "CWeapon.h"

////////////////////Components/////////
#include "CBounding_AABB.h"
#include "CBoxColliderComponent.h"
#include "CCollider_Base.h"
#include "CNavigation.h"
#include "CCell.h"

//////////////States///////////
#include "MonsterStates.h"
#include "CMMoriblinSwordIdleState.h"
#include "CMMoriblinSwordDetectState.h"
#include "CMMoriblinSwordGuardState.h"
#include "CMMoriblinAttackState.h"



USING(Client)
CM_MoriblinSword::CM_MoriblinSword(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CMonster(pDevice,pContext)
{
}

CM_MoriblinSword::CM_MoriblinSword(const CM_MoriblinSword& rhs)
	: CMonster(rhs)
{
}

CM_MoriblinSword::~CM_MoriblinSword()
{
}

HRESULT CM_MoriblinSword::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;



	return S_OK;
}


HRESULT CM_MoriblinSword::Initialize_Copytype(void* pArg)
{
	if (FAILED(__super::Initialize_Copytype(pArg)))
		return E_FAIL;

	m_iEventHandle=CGameInstance::GetInstance()->RegisterListners("Enter_Forest", [this](const GameEvent& event)
		{
			Set_Active(true);
		});


	Register_Anim();

	if (FAILED(Ready_Component(pArg)))
		return E_FAIL;

	if (FAILED(Ready_States()))
		return E_FAIL;

	if (FAILED(Ready_WeaponColliders()))
		return E_FAIL;


	if (m_pNavigationCom)
	{
		m_pTransformCom->Set_State(STATE::POSITION, m_pNavigationCom->Get_Cell(m_iHomeCell)->Get_CenterPos());
		m_pNavigationCom->Set_CurrentIdx(m_pTransformCom->Get_State(STATE::POSITION));
		m_pNavigationCom->Get_RandomCells(m_pTransformCom->Get_State(STATE::POSITION),
			m_fRoamRadius, &m_RandomCells);


	}

	Change_State(ENUM_TO_UINT(CMonster::MONSTER_BASE_STATE::IDLE));

	return S_OK;
}

void CM_MoriblinSword::Register_Anim()
{
	if (m_pMonsterBody)
	{
		m_pMonsterBody->Register_AnimKey(ENUM_TO_UINT(CMonster::MONSTER_BASE_STATE::IDLE), L"wait");
		m_pMonsterBody->Register_AnimKey(ENUM_TO_UINT(CM_MoriblinSword::MoriblinState::DETECT), L"find");
		m_pMonsterBody->Register_AnimKey(ENUM_TO_UINT(CMonster::MONSTER_BASE_STATE::DIE), L"dead_");

		m_pMonsterBody->Register_AnimKey(ENUM_TO_UINT(CMonster::MONSTER_BASE_STATE::ATTACK), L"stance_walk");
		m_pMonsterBody->Register_AnimKey(ENUM_TO_UINT(CMonster::MONSTER_BASE_STATE::RUN), L"stance_walk");

		m_pMonsterBody->Register_AnimKey(ENUM_TO_UINT(CM_MoriblinSword::MoriblinState::GUARD), L"guard");

		m_pMonsterBody->Register_AnimKey(ENUM_TO_UINT(CMonster::MONSTER_BASE_STATE::DAMAGE), L"damage_");

		m_pMonsterBody->Register_AnimKey(ENUM_TO_UINT(CMonster::MONSTER_BASE_STATE::WALK), L"walk");



		//////////Speed///////
		m_pMonsterBody->Set_Animation_Speed(L"walk", 50.f);
		m_pMonsterBody->Set_Animation_Speed(L"find", 50.f);
		m_pMonsterBody->Set_Animation_Speed(L"stance_walk", 50.f);


	}
}

HRESULT CM_MoriblinSword::Ready_Component(void* pArg)
{
	//////////////Boxcollider추가
	CCollider_Base::COLLIDER_DESC pColliderDesc;
	pColliderDesc.m_eColGroup = ENUM_TO_UINT(COLLISION_GROUP::MONSTER);
	pColliderDesc.m_iLevelID = m_iLevelID;

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
	
	m_pCollider->Set_Trigger(false);

	return  S_OK;
}

HRESULT CM_MoriblinSword::Ready_States()
{
	m_States.emplace(CMonster::MONSTER_BASE_STATE::IDLE, CMMoriblinSwordIdleState::Create());
	m_States.emplace(CMonster::MONSTER_BASE_STATE::RUN, CMonsterRunState::Create());
	m_States.emplace(CMonster::MONSTER_BASE_STATE::WALK, CMonsterWalkState::Create());


	m_States.emplace(CMonster::MONSTER_BASE_STATE::ATTACK, CMMoriblinAttackState::Create());
	m_States.emplace(CM_MoriblinSword::MoriblinState::DETECT, CMMoriblinSwordDetectState::Create());

	m_States.emplace(CM_MoriblinSword::MoriblinState::GUARD, CMMoriblinSwordGuardState::Create());

	m_States.emplace(CMonster::MONSTER_BASE_STATE::DAMAGE, CMonsterDamageState::Create());
	m_States.emplace(CMonster::MONSTER_BASE_STATE::DIE, CMonsterDeadState::Create());






	return S_OK;
}

HRESULT CM_MoriblinSword::Ready_WeaponColliders()
{

	CMMoriblin_Weapon::tagMoriblinWeapon SwordDesc{};
	SwordDesc.pOwner = this;
	SwordDesc.pSocketMatrix= m_pBody->Get_SocketMatrix("attach_L");
	SwordDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	SwordDesc.AnimKey = L"";	//단지 튕겨내기
	SwordDesc.fRadius = 0.3f;
	SwordDesc.vOffSet= _float3(0.f, 0.f, 0.f);
	SwordDesc.m_iLevelID = m_iLevelID;

	if (FAILED(__super::Add_PartObject(0, PROTO_OBJ_NAME(L"Moriblin_Weapon"), L"Moriblin_Sword", &SwordDesc)))
		return E_FAIL;


	CMMoriblin_Weapon::tagMoriblinWeapon ShieldDesc{};
	ShieldDesc.pOwner = this;
	ShieldDesc.pSocketMatrix = m_pBody->Get_SocketMatrix("attach_R");
	ShieldDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	ShieldDesc.AnimKey = L"guard";	//가드애니메이션
	ShieldDesc.fRadius = 0.2f;
	ShieldDesc.m_iLevelID = m_iLevelID;

	if (FAILED(__super::Add_PartObject(0, PROTO_OBJ_NAME(L"Moriblin_Weapon"), L"Moriblin_Shield", &ShieldDesc)))
		return E_FAIL;

	m_pWeapons.resize(2);

	m_pWeapons[0] = dynamic_cast<CWeapon*>(Find_PartObject(L"Moriblin_Shield"));
	m_pWeapons[1] = dynamic_cast<CWeapon*>(Find_PartObject(L"Moriblin_Sword"));

	return S_OK;
}

string CM_MoriblinSword::Convert_String_To_Enum(_uint eState)
{
	string StateDebugStr = "";

	if (eState == 0)
		return "NONE";


	else
	{
		if (eState == ENUM_TO_UINT(CMonster::MONSTER_BASE_STATE::INTRO))
			StateDebugStr += "INTRO ";

		if (eState == ENUM_TO_UINT(CMonster::MONSTER_BASE_STATE::IDLE))
			StateDebugStr += "IDLE ";

		if (eState == ENUM_TO_UINT(CMonster::MONSTER_BASE_STATE::WALK))
			StateDebugStr += "WALK";

		if (eState == ENUM_TO_UINT(CMonster::MONSTER_BASE_STATE::RUN))
			StateDebugStr += "RUN";

		if (eState == ENUM_TO_UINT(CMonster::MONSTER_BASE_STATE::JUMP))
			StateDebugStr += "JUMP ";

		if (eState == ENUM_TO_UINT(CMonster::MONSTER_BASE_STATE::ATTACK))
			StateDebugStr += "ATTACK ";

		if (eState == ENUM_TO_UINT(CMonster::MONSTER_BASE_STATE::DAMAGE))
			StateDebugStr += "DAMAGE ";

		if (eState == ENUM_TO_UINT(CMonster::MONSTER_BASE_STATE::DIE))
			StateDebugStr += "DIE";

		if (eState == ENUM_TO_UINT(CM_MoriblinSword::MoriblinState::DETECT))
			StateDebugStr += "DETECT";


		if (eState == ENUM_TO_UINT(CM_MoriblinSword::MoriblinState::GUARD))
			StateDebugStr += "GUARD";
	}



	return StateDebugStr;
}

void CM_MoriblinSword::Idle_Behavior(float fTimeDelta)
{
	if (m_ActionControl.m_bThink)
	{
		m_fWaitCurrentTime += fTimeDelta;
		if (m_fWaitCurrentTime >= m_fWaitTime)
		{
			m_ActionControl.m_bThink = false;
			m_fWaitCurrentTime = 0.f;
		}
	}


	/*거리만족 해야 Detect를 할수있다.*/
	if (Is_InRange(m_fDetectRange))
	{
		CheckTrue(m_bDetect);	//이전에 감지했다면,,또감지 하지않는다.

		/*시야범위내에 있는지 판단하기*/
		CGameObject* pPlayer = m_pGameInstance->Find_GameObject(0, L"Player_Layer", L"Player");
		CTransform* pPlayerTrans = pPlayer->Get_Transform();

		_vector vDir = pPlayerTrans->Get_State(STATE::POSITION) - m_pTransformCom->Get_State(STATE::POSITION);
		m_bDetect = m_pTransformCom->IsInSight(60.f, vDir);
		if (m_bDetect)
		{
			m_pTarget = pPlayer;
			m_pTransformCom->LookAt(pPlayerTrans->Get_State(STATE::POSITION));
			m_ActionControl.m_bChase = true;
		}
	}

	/*아니라면 그냥 이동상태.*/
	else
	{
		//Think상태일떄는 움직이지말자.
		CheckTrue(m_ActionControl.m_bThink);
		

		//이동할 다음 렌덤 셀 선별
		m_ActionControl.m_bMove = true;

		int RandIdx = rand() % m_RandomCells.size();
		m_iNextCell = m_RandomCells[RandIdx];
	}
	
}

void CM_MoriblinSword::Chase_Behavior(float fTimeDelta)
{
	CheckNull(m_pTarget);
	m_pTransformCom->LookAt(m_pTarget->Get_Transform());

	//거리멀어지면 idle상태로 다시바꾸기
	if (!Is_InRange(m_fDetectRange))
	{
		m_bDetect = false;
		m_pTarget = nullptr;
		m_ActionControl.m_bRange = false;
		m_ActionControl.m_bAttack = false;
		m_ActionControl.m_bChase = false;
		return;
	}

	
	m_ActionControl.m_bRange = true;
	_vector vPoint = m_pTarget->Get_Transform()->Get_State(STATE::POSITION);

	bool isReach=m_pTransformCom->Chase(vPoint, fTimeDelta,m_pNavigationCom, 2.f);



	if (isReach)
		m_ActionControl.m_bAttack = true;

}

void CM_MoriblinSword::Attack_Behavior(float fTimeDelta)
{
	if (!m_pTarget)
	{
		m_ActionControl.m_bAttack = false;
		return;
	}


	m_pTransformCom->LookAt(m_pTarget->Get_Transform());

	
	//거리멀어지면  chase상태로 다시바꾸기,타겟유지
	if (!Is_InRange(fActionRange))
	{
		m_ActionControl.m_bAttack = false;
	}
		

}

void CM_MoriblinSword::Patrol_Behavior(float fTimeDelta)
{
	CCell* pCell = m_pNavigationCom->Get_Cell(m_iNextCell);
	_vector CellCenter = pCell->Get_CenterPos();

	m_pTransformCom->LookAt(WORLD_UP,CellCenter,fTimeDelta);
	bool bReach= m_pTransformCom->Chase(CellCenter, fTimeDelta, m_pNavigationCom, 2.f);

	if (bReach)
	{
		m_ActionControl.m_bMove = false;
		m_fReachTime = 0.f;
	}
	else
		m_fReachTime += fTimeDelta;
	
	/*4초안에 도달못할경우 강제로 셀 변경 */
	if (m_fReachTime >=2.5f)
	{
		int RandIdx = rand() % m_RandomCells.size();
		m_iNextCell = m_RandomCells[RandIdx];
		m_fReachTime = 0.f;
		m_ActionControl.m_bMove = false;

	}
	if (Is_InRange(m_fDetectRange))
	{
		CheckTrue(m_bDetect);	//이전에 감지했다면,,또감지 하지않는다.

		/*시야범위내에 있는지 판단하기*/
		CGameObject* pPlayer = m_pGameInstance->Find_GameObject(0, L"Player_Layer", L"Player");
		CTransform* pPlayerTrans = pPlayer->Get_Transform();

		_vector vDir = pPlayerTrans->Get_State(STATE::POSITION) - m_pTransformCom->Get_State(STATE::POSITION);
		m_bDetect = m_pTransformCom->IsInSight(60.f, vDir);
		if (m_bDetect)
		{
			m_pTarget = pPlayer;
			m_pTransformCom->LookAt(pPlayerTrans->Get_State(STATE::POSITION));
			m_ActionControl.m_bChase = true;
			m_ActionControl.m_bMove = false;
		}
	}

	
}

void CM_MoriblinSword::Set_CollisionEnable(bool _b)
{
	__super::Set_CollisionEnable(false);

	for (auto& collision : m_pWeapons)
		collision->Set_Active(false);


}

void CM_MoriblinSword::Enter_State(int newState)
{
	//Idle 진입상태시 약간의 대기시간
	if (newState == CMonster::IDLE)
	{
		//현재 위치가 처음 스폰장소보다 많이 떨어져있따면 다시갱신

		
		//감지판정 ,시간 리셋
		m_bDetect = false;
		m_pTarget = nullptr;
		m_ActionControl.Reset();
		m_fWaitCurrentTime = 0.f;


		//잠깐대기시간
		m_ActionControl.m_bThink = true;
		m_fReachTime = 0.f;

	}

	if (newState == CMonster::DAMAGE)
	{
		//잠깐 weapon콜라이더 비활성화.
		for (int i = 0; i < m_pWeapons.size(); ++i)
			m_pWeapons[i]->Set_Active(false);

	}

	if (newState == CMonster::DIE)
	{
		m_pAnimBody->Set_PassName("Dissolve");
		
	}
}

void CM_MoriblinSword::Exit_State(int newState)
{
	for (int i = 0; i < m_pWeapons.size(); ++i)
		m_pWeapons[i]->Set_Active(true);

}

void CM_MoriblinSword::AIState_Change(_float fTimeDelta)
{         
	Update_DeadState(fTimeDelta);

	CheckTrue(m_ActionControl.m_bDead);

	switch (m_iState)
	{
	case Client::CMonster::MONSTER_BASE_STATE::IDLE:
		//Detect판정
		Idle_Behavior(fTimeDelta);
		break;

	case Client::CMonster::MONSTER_BASE_STATE::WALK:
		Patrol_Behavior(fTimeDelta);
		break;
	case Client::CMonster::MONSTER_BASE_STATE::RUN:
		Chase_Behavior(fTimeDelta);
		break;
	case Client::CMonster::MONSTER_BASE_STATE::JUMP:
		break;
	case Client::CMonster::MONSTER_BASE_STATE::ATTACK:
		Attack_Behavior(fTimeDelta);
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

void CM_MoriblinSword::Update_Movement(_float fTimeDelta)
{
	
	m_pTransformCom->Set_State(STATE::POSITION,
		m_pNavigationCom->SetUp_OnNavigation(m_pTransformCom->Get_State(STATE::POSITION)));

	CheckTrue(m_ActionControl.m_bDead);


}





void CM_MoriblinSword::Update_Priority(_float fTimeDelta)
{
	__super::Update_Priority(fTimeDelta);
}

void CM_MoriblinSword::Update(_float fTimeDelta)
{
	AIState_Change(fTimeDelta);


	if (m_pCurState)
		m_pCurState->Update(this, fTimeDelta);


	__super::Update(fTimeDelta);
}

void CM_MoriblinSword::Update_Late(_float fTimeDelta)
{
	__super::Update_Late(fTimeDelta);


	if (m_pCurState)
		m_pCurState->Update_Late(this, fTimeDelta);
	m_pCollider->Update_Collider(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));

}

void CM_MoriblinSword::Update_Render(_float fTimeDelta)
{
	__super::Update_Render(fTimeDelta);
}



HRESULT CM_MoriblinSword::Render()
{
	__super::Render();



	return S_OK;
}

CM_MoriblinSword* CM_MoriblinSword::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
{
	CM_MoriblinSword* pInstance = new CM_MoriblinSword(_pDevice, _pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create :CM_MoriblinSword ");
		Safe_Release(pInstance);

	}



	return pInstance;
}

CGameObject* CM_MoriblinSword::Clone(void* pArg)
{
	CM_MoriblinSword* pInstance = new CM_MoriblinSword(*this);
	if (FAILED(pInstance->Initialize_Copytype(pArg)))
	{
		MSG_BOX("Failed to Cloned :CMonster ");
		Safe_Release(pInstance);

	}
	return pInstance;
}

void CM_MoriblinSword::Free()
{
	__super::Free();
}

void CM_MoriblinSword::Set_Dead()
{

	__super::Set_Dead();
	m_pGameInstance->UnRegisterListenrs("Enter_Forest", m_iEventHandle);
}

void CM_MoriblinSword::OnCollisionEnter(_uint iGroup, CCollider_Base* pOther)
{
	CheckFalse(m_bCanCollision);
	CheckTrue(m_bGuard);
	CheckTrue(m_ActionControl.m_bDamage == 1.f);

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
			m_pTransformCom->AddImpulse(-0.3f, vDir);

			_float4x4 CombinedMatrix = pPart->Get_CombinedWorldMatrix();
			Spawn_HitSparkle(XMLoadFloat4x4(&CombinedMatrix));
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
		
	

	}
		break;
	
	}
	
	
	
}


void CM_MoriblinSword::Push_Behavior(CGameObject* pOther)
{
	__super::Push_Behavior(pOther);
	
	m_bGuard = false;
	m_pMonsterBody->Change_AnimKey(ENUM_TO_UINT(CMonster::MONSTER_BASE_STATE::DAMAGE), L"damage_f");
	Change_State(CMonster::MONSTER_BASE_STATE::DAMAGE);

}



void CM_MoriblinSword::Set_Guard(bool b, wstring AnimKey)
{
	m_bGuard = b;
	m_pMonsterBody->Register_AnimKey(ENUM_TO_UINT(CM_MoriblinSword::GUARD), AnimKey);

}

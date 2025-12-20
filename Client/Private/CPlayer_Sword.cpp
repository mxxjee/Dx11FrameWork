#include "CPlayer_Sword.h"
#include "CSphereColliderComponent.h"
#include "CBounding_Sphere.h"
#include "CGameInstance.h"
#include "Client_Defines.h"

USING(Client)
CPlayer_Sword::CPlayer_Sword(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CWeapon(pDevice,pContext)
{
}

CPlayer_Sword::CPlayer_Sword(const CPlayer_Sword& Prototype)
	:CWeapon(Prototype)
{
}

HRESULT CPlayer_Sword::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;


	return S_OK;
}

HRESULT CPlayer_Sword::Initialize_Copytype(void* pArg)
{
	m_pCollider.resize(ENUM_TO_UINT(CollisionPos::END));


	if (FAILED(__super::Initialize_Copytype(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components(pArg)))
		return E_FAIL;

	
	m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(0.f, 0.f, -0.6f, 1.f));


	Set_Active(false);
	return S_OK;
}

void CPlayer_Sword::Update_Priority(_float fTimeDelta)
{
	__super::Update_Priority(fTimeDelta);
}

void CPlayer_Sword::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);

}

void CPlayer_Sword::Update_Late(_float fTimeDelta)
{
	__super::Update_Late(fTimeDelta);
}

void CPlayer_Sword::Update_Render(_float fTimeDelta)
{
	__super::Update_Render(fTimeDelta);
}

HRESULT CPlayer_Sword::Render()
{
	__super::Render();

	return S_OK;
}

void CPlayer_Sword::OnCollisionEnter(_uint iGroup, CCollider_Base* pOther)
{
	
}

void CPlayer_Sword::OnCollisionStay(_uint iGroup, CCollider_Base* pOther)
{

	
}

void CPlayer_Sword::OnCollisionExit(_uint iGroup, CCollider_Base* pOther)
{

}

HRESULT CPlayer_Sword::Ready_Components(void* pArg)
{
	/* For.Com_Collider_Sphere */


	CCollider_Base::COLLIDER_DESC pColliderDesc;
	pColliderDesc.m_eColGroup = ENUM_TO_UINT(COLLISION_GROUP::PLAYER_WEAPON);
	pColliderDesc.pOwner = this;
	pColliderDesc.m_iLevelID = ENUM_TO_UINT(LEVEL_ID::STATIC);


	for (int i = 0; i < CollisionPos::END; ++i)
	{
		CBounding_Sphere::BOUNDING_SPHERE_DESC      CollDesc{};
		CollDesc.fRadius = 0.4f;
		CollDesc.vCenter = _float3(0.f,  0.f, (CollDesc.fRadius*2) * i);

		pColliderDesc.m_BoundingDesc = &CollDesc;
		


		CComponent* pCollider = dynamic_cast<CSphereColliderComponent*>(m_pGameInstance->Clone_Prototype(
			PROTOTYPE::COMPONENT,
			0,
			PROTO_COMPONENT_NAME(L"SphereCollider"),
			&pColliderDesc)
			);

		m_pCollider[i] = dynamic_cast<CCollider_Base*>(pCollider);
		pCollider->Set_Owner(this);

	}



	return S_OK;
}

CPlayer_Sword* CPlayer_Sword::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	CPlayer_Sword* pInstance = new CPlayer_Sword(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CPlayer_Sword");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CPlayer_Sword::Clone(void* pArg)
{
	CPlayer_Sword* pInstance = new CPlayer_Sword(*this);

	if (FAILED(pInstance->Initialize_Copytype(pArg)))
	{
		MSG_BOX("Failed to Cloned : CPlayer_Sword");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_Sword::Free()
{
	__super::Free();

}


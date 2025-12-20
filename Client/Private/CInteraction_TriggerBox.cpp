#include "CInteraction_TriggerBox.h"
#include "Client_Defines.h"
#include "CCollider_Base.h"
#include "CGameInstance.h"
#include "CBounding_AABB.h"
#include "CBoxColliderComponent.h"

USING(Client)
CInteraction_TriggerBox::CInteraction_TriggerBox(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CPartObject(pDevice, pContext)
{
}

CInteraction_TriggerBox::CInteraction_TriggerBox(const CInteraction_TriggerBox& Prototype)
	: CPartObject(Prototype)
{
}

HRESULT CInteraction_TriggerBox::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CInteraction_TriggerBox::Initialize_Copytype(void* pArg)
{
	INTERACTION_TRIGGER_DESC* pDesc = static_cast<INTERACTION_TRIGGER_DESC*>(pArg);


	if (FAILED(__super::Initialize_Copytype(pArg)))
		return E_FAIL;


	if (FAILED(Ready_Component(pArg)))
		return E_FAIL;



	return S_OK;
}

void CInteraction_TriggerBox::Update_Priority(_float fTimeDelta)
{
	__super::Update_Priority(fTimeDelta);
}

void CInteraction_TriggerBox::Update(_float fTimeDelta)
{
	
	__super::SetUp_CombinedWorldMatrix(XMLoadFloat4x4(m_pParentMatrix));
}

void CInteraction_TriggerBox::Update_Late(_float fTimeDelta)
{
	for (auto& i : m_pCollider)
		i->Update_Collider(XMLoadFloat4x4(&m_CombinedWorldMatrix));

}

void CInteraction_TriggerBox::Update_Render(_float fTimeDelta)
{
	m_pGameInstance->Add_RenderObject(ENUM_TO_UINT(RENDERGROUP::NONALPHA), this);

}

HRESULT CInteraction_TriggerBox::Render()
{
#ifdef _DEBUG
	if (CGameInstance::m_bDrawDebug)
	{
		for (auto& i : m_pCollider)
			i->Render();
	}

#endif
	return S_OK;
}

HRESULT CInteraction_TriggerBox::Ready_Component(void* pArg)
{
	INTERACTION_TRIGGER_DESC* pDesc = static_cast<INTERACTION_TRIGGER_DESC*>(pArg);

	CCollider_Base::COLLIDER_DESC pColliderDesc;
	pColliderDesc.m_eColGroup = ENUM_TO_UINT(COLLISION_GROUP::INTERACTION);
	pColliderDesc.pOwner = this;
	pColliderDesc.m_iLevelID = pDesc->m_iLevelID;

	CBounding_AABB::BOUNDING_AABB_DESC      CollDesc{};
	CollDesc.Extents = pDesc->vExtents;
	CollDesc.vCenter = pDesc->vCenter;

	pColliderDesc.m_BoundingDesc = &CollDesc;



	CComponent* pCollider = dynamic_cast<CBoxColliderComponent*>(m_pGameInstance->Clone_Prototype(
		PROTOTYPE::COMPONENT,
		0,
		PROTO_COMPONENT_NAME(L"BoxCollider"),
		&pColliderDesc)
		);

	m_pCollider[0] = dynamic_cast<CCollider_Base*>(pCollider);
	pCollider->Set_Owner(this);

	return S_OK;
}

void CInteraction_TriggerBox::Set_Active(bool _b)
{
	m_bActive = _b;
	for (auto& pCol : m_pCollider)
		pCol->Set_Active(_b);
}

void CInteraction_TriggerBox::Set_Size(_float3 vSize)
{
	CheckTrue(m_pCollider.empty());

	for (auto& pCol : m_pCollider)
	{
		CBounding_AABB* Bound_AABB = dynamic_cast<CBounding_AABB*>(pCol->Get_Bounding());
		CheckNull(Bound_AABB);

		Bound_AABB->Set_Size(vSize);
	}
	
}

void CInteraction_TriggerBox::OnCollisionEnter(_uint iGroup, CCollider_Base* pOther)
{
	CGameObject* pOwner = pOther->Get_Owner();
	CheckNull(pOwner);


	switch (COLLISION_GROUP(iGroup))
	{
	case Client::COLLISION_GROUP::PLAYER:
		m_bCollision = true;

		break;



	}
}

void CInteraction_TriggerBox::OnCollisionStay(_uint iGroup, CCollider_Base* pOther)
{
}

void CInteraction_TriggerBox::OnCollisionExit(_uint iGroup, CCollider_Base* pOther)
{

	CGameObject* pOwner = pOther->Get_Owner();
	CheckNull(pOwner);


	switch (COLLISION_GROUP(iGroup))
	{
	case Client::COLLISION_GROUP::PLAYER:
		m_bCollision = false;

	break;



	}
}

CInteraction_TriggerBox* CInteraction_TriggerBox::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
	CInteraction_TriggerBox* pInstance = new CInteraction_TriggerBox(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CInteraction_TriggerBox");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CInteraction_TriggerBox::Clone(void* pArg)
{
	CInteraction_TriggerBox* pInstance = new CInteraction_TriggerBox(*this);

	if (FAILED(pInstance->Initialize_Copytype(pArg)))
	{
		MSG_BOX("Failed to Cloned : CInteraction_TriggerBox");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CInteraction_TriggerBox::Free()
{
	for (auto& i : m_pCollider)
		Safe_Release(i);

	__super::Free();

}

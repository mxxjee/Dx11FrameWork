#include "CInteraction_TriggerBox.h"
#include "Client_Defines.h"
#include "CIInteractable.h"
#include "CContainerObject.h"
#include "CCollider_Base.h"
#include "CGameInstance.h"
#include "CBounding_AABB.h"
#include "CBoxColliderComponent.h"
#include <algorithm>

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

void CInteraction_TriggerBox::Register_Colliders(_uint iLevelIdx)
{
	for (auto& pCol : m_pCollider)
		m_pGameInstance->Register_Collider(pCol, iLevelIdx);
}

HRESULT CInteraction_TriggerBox::Ready_Component(void* pArg)
{
	INTERACTION_TRIGGER_DESC* pDesc = static_cast<INTERACTION_TRIGGER_DESC*>(pArg);

	CCollider_Base::COLLIDER_DESC pColliderDesc;
	pColliderDesc.m_eColGroup = ENUM_TO_UINT(COLLISION_GROUP::TRIGGER);
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
	if (!_b)
		Clear_Overlaps();

	m_bActive = _b;
	for (auto& pCol : m_pCollider)
		pCol->Set_Active(_b);
}

CGameObject* CInteraction_TriggerBox::Get_PlayerOther() const
{
	if (m_PlayerOverlaps.empty() || !m_PlayerOverlaps.back())
		return nullptr;

	return m_PlayerOverlaps.back()->Get_Owner();
}

CGameObject* CInteraction_TriggerBox::Get_MonsterOther() const
{
	if (m_MonsterOverlaps.empty() || !m_MonsterOverlaps.back())
		return nullptr;

	return m_MonsterOverlaps.back()->Get_Owner();
}

void CInteraction_TriggerBox::Add_Overlap(std::vector<CCollider_Base*>& Overlaps, CCollider_Base* pOther)
{
	CheckNull(pOther);

	if (std::find(Overlaps.begin(), Overlaps.end(), pOther) == Overlaps.end())
		Overlaps.push_back(pOther);
}

void CInteraction_TriggerBox::Remove_Overlap(std::vector<CCollider_Base*>& Overlaps, CCollider_Base* pOther)
{
	auto Iter = std::find(Overlaps.begin(), Overlaps.end(), pOther);
	if (Iter != Overlaps.end())
		Overlaps.erase(Iter);
}

void CInteraction_TriggerBox::Clear_Overlaps()
{
	if (!m_PlayerOverlaps.empty())
	{
		CIInteractable* pInteractable = dynamic_cast<CIInteractable*>(m_pOwner);
		if (pInteractable)
			pInteractable->OnInteractionRangeExit();
	}

	m_PlayerOverlaps.clear();
	m_MonsterOverlaps.clear();
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
	switch (COLLISION_GROUP(iGroup))
	{
	case Client::COLLISION_GROUP::PLAYER:
	{
		const bool bWasEmpty = m_PlayerOverlaps.empty();
		Add_Overlap(m_PlayerOverlaps, pOther);

		if (bWasEmpty && !m_PlayerOverlaps.empty())
		{
			CIInteractable* pInteractable = dynamic_cast<CIInteractable*>(m_pOwner);
			if (pInteractable)
				pInteractable->OnInteractionRangeEnter();
		}
		break;
	}

	case Client::COLLISION_GROUP::MONSTER:
		Add_Overlap(m_MonsterOverlaps, pOther);
		break;

	}
}

void CInteraction_TriggerBox::OnCollisionStay(_uint iGroup, CCollider_Base* pOther)
{
	// Keep overlap state valid even when a collider is re-enabled while already overlapping.
	OnCollisionEnter(iGroup, pOther);
}

void CInteraction_TriggerBox::OnCollisionExit(_uint iGroup, CCollider_Base* pOther)
{

	switch (COLLISION_GROUP(iGroup))
	{
	case Client::COLLISION_GROUP::PLAYER:
	{
		const bool bWasOverlapping = !m_PlayerOverlaps.empty();
		Remove_Overlap(m_PlayerOverlaps, pOther);
		if (bWasOverlapping && m_PlayerOverlaps.empty())
		{
			CIInteractable* pInteractable = dynamic_cast<CIInteractable*>(m_pOwner);
			if (pInteractable)
				pInteractable->OnInteractionRangeExit();
		}
	break;
	}

	case Client::COLLISION_GROUP::MONSTER:
		Remove_Overlap(m_MonsterOverlaps, pOther);
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
	// The owning CIInteractable purges Manager state before its parts are freed.
	// Do not enqueue a new Remove request from this destruction path.
	m_PlayerOverlaps.clear();
	m_MonsterOverlaps.clear();

	for (auto& i : m_pCollider)
	{
		m_pGameInstance->UnRegister_Collider(i, m_iSceneID);

		Safe_Release(i);
	
	}

	__super::Free();

}

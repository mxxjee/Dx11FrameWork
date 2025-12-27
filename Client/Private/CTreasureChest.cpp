#include "CTreasureChest.h"
#include "CAnimBody.h"
#include "CCollider_Base.h"
#include "CBounding_AABB.h"
#include "CBoxColliderComponent.h"
#include "CNavigation.h"

#include "CInteraction_TriggerBox.h"
#include "CInteraction_Manager.h"

#include "CModel.h"



USING(Client)
CTreasureChest::CTreasureChest(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CInteractionObject(pDevice,pContext)
{
}

CTreasureChest::CTreasureChest(const CTreasureChest& rhs)
	: CInteractionObject(rhs)
{
}

HRESULT CTreasureChest::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTreasureChest::Initialize_Copytype(void* pArg)
{
	TREASURECHEST_DESC* pDesc = static_cast<TREASURECHEST_DESC*>(pArg);
	pDesc->eInteractionType = ENUM_TO_UINT(InteractionType::OBJECT);
	pDesc->eInteract_Object_Type = InteractionType::OBJECT;
	//pDesc.m_iLevelID = ENUM_TO_UINT(LEVEL_ID::ROOM);


	pDesc->eRenderGroup = ENUM_TO_UINT(RENDERGROUP::NONALPHA);
	pDesc->ModelName = L"TreasureBox_Top";
	pDesc->bAnimated = true;

	CBoxColliderComponent::COLLIDER_DESC ColDesc;
	ColDesc.m_eColGroup = ENUM_TO_UINT(COLLISION_GROUP::INTERACTION);
	CBounding_AABB::BOUNDING_AABB_DESC aabbDesc;
	aabbDesc.vCenter = _float3(0.f, 0.f, 0.f);
	aabbDesc.Extents = _float3(0.5f, 0.5f, 1.f);
	ColDesc.m_BoundingDesc = &aabbDesc;
	ColDesc.pOwner = this;
	ColDesc.m_iLevelID = m_iSceneID;
	pDesc->pColliderComp = &ColDesc;


	m_pInnerItem = pDesc->m_pInnerItem;

	if (FAILED(__super::Initialize_Copytype(pArg)))
		return E_FAIL;


	m_pCollider->Set_Trigger(false);
	m_BehaviorType = Interact_Behavior_Type::PUSHABLE;

	CInteraction_Manager::GetInstance()->RegisterInteractable(this);

	CAnimBody* pAnimBody = dynamic_cast<CAnimBody*>(m_pBody);
	CheckNullResult(pAnimBody,E_FAIL);

	pAnimBody->Get_Model()->Set_Animation(L"wait", true);

	return S_OK;
}

HRESULT CTreasureChest::Ready_PartObjects(void* pArg)
{
	
	if (FAILED(__super::Ready_PartObjects(pArg)))
		return E_FAIL;

	////bottom
	CAnimBody::ANIMBODY_DESC pAnimBodyDesc;
	pAnimBodyDesc.eRenderGroup = m_eRenderGroup;
	pAnimBodyDesc.pOwner = this;
	pAnimBodyDesc.modelName = L"TreasureBox_Bottom";
	pAnimBodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	pAnimBodyDesc.pParentState = nullptr;
	pAnimBodyDesc.ObjTag = L"TreasureBox_Bottom_body";
	if (FAILED(__super::Add_PartObject(0, PROTO_OBJ_NAME(L"AnimBody"), L"TreasureBox_Bottom_body", &pAnimBodyDesc)))
		return E_FAIL;

	m_pBodyBottom = dynamic_cast<CBody*>(Find_PartObject(L"Part_Body"));

	if (m_pBodyBottom)
		Safe_AddRef(m_pBodyBottom);


	Interaction_DESC* pDesc = static_cast<Interaction_DESC*>(pArg);


	CInteraction_TriggerBox::tagInteractionTriggerBoxDesc TriggerDesc;
	TriggerDesc.ObjTag = pDesc->ObjTag + L"Trigger_Box";
	TriggerDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	TriggerDesc.pOwner = this;
	TriggerDesc.m_iLevelID = m_iSceneID;

	CBounding_AABB* pAABB = dynamic_cast<CBounding_AABB*>(m_pCollider->Get_Bounding());

	if (pAABB)
	{
		BoundingBox* pBoundingBox = pAABB->Get_OrignialDesc();
		TriggerDesc.vCenter = _float3(0.f, 0.f, 0.f);
		TriggerDesc.vExtents = _float3(2.f, 2.f, 2.f);

		if (FAILED(__super::Add_PartObject(0, PROTO_OBJ_NAME(L"Interaction_TriggerBox"), L"Part_TriggerBox", &TriggerDesc)))
			return E_FAIL;

		m_pTriggerBox = dynamic_cast<CInteraction_TriggerBox*>(Find_PartObject(L"Part_TriggerBox"));

	}


	return S_OK;
}

void CTreasureChest::Update_Priority(_float fTimeDelta)
{
	__super::Update_Priority(fTimeDelta);
}

void CTreasureChest::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
	if (m_pBody->Get_Model()->Get_CurrentAnimKey() == L"open" && m_pBody->Get_Model()->Get_IsAnimFinished())
	{
		m_pBody->Get_Model()->Set_Animation(L"open_wait", true);
	}
}

void CTreasureChest::Update_Late(_float fTimeDelta)
{
	__super::Update_Late(fTimeDelta);
	m_pBody->Get_Model()->Play_Animation(fTimeDelta);

	m_pCollider->Update_Collider(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));
}

void CTreasureChest::Update_Render(_float fTimeDelta)
{
	__super::Update_Render(fTimeDelta);
	m_pGameInstance->Add_RenderObject(ENUM_TO_UINT(RENDERGROUP::NONALPHA), this);

#ifdef _DEBUG
	if (CGameInstance::m_bDrawDebug)
	{
		if (FAILED(m_pGameInstance->Add_DebugComponent(m_pCollider)))
			return;
	}
#endif
}




HRESULT CTreasureChest::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

CTreasureChest* CTreasureChest::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContex)
{
	CTreasureChest* pInstance = new CTreasureChest(_pDevice, _pDeviceContex);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CTreasureChest ");
		Safe_Release(pInstance);

	}

	return pInstance;
}


CGameObject* CTreasureChest::Clone(void* pArg)
{
	CTreasureChest* pInstance = new CTreasureChest(*this);
	if (FAILED(pInstance->Initialize_Copytype(pArg)))
	{
		MSG_BOX("Failed to Clone :CTreasureChest ");
		Safe_Release(pInstance);

	}



	return pInstance;
}



bool CTreasureChest::IsInteratable()
{

	CheckNullResult(m_pTriggerBox, false);
	return m_pTriggerBox->Is_Collision();

}

void CTreasureChest::Enter_InteractRange()
{
	CheckTrue(m_bInteraction);

	_vector ShowPos = MathUtils::WorldToScreen(m_pTransformCom->Get_State(STATE::POSITION),
		m_pGameInstance->Get_ViewMatrix(0), m_pGameInstance->Get_ProjMatrix(0), g_iWinSizeX, g_iWinSizeY);


	m_pGameInstance->BroadCastEvent(L"OnSeeUIShow", (void*)&ShowPos);

}

void CTreasureChest::Stay_InteractRange(_float fTimeDelta)
{
}

void CTreasureChest::Exit_InteractRange()
{
	CheckTrue(m_bInteraction);
	m_pGameInstance->BroadCastEvent(L"OnSeeUIHide", (void*)nullptr);


}

void CTreasureChest::Enter_Interaction()
{
	CheckTrue(m_bOpen);

	CAnimBody* pAnimBody = dynamic_cast<CAnimBody*>(m_pBody);
	CheckNull(pAnimBody);

	pAnimBody->Get_Model()->Set_Animation(L"open", false);
	m_bOpen = true;



}

void CTreasureChest::Stay_Interaction(_float fTimeDelta)
{
}

void CTreasureChest::Exit_Interaction()
{
}

_int CTreasureChest::Get_Interaction_Priority()
{
	return InteractionType::OBJECT;
}


void CTreasureChest::Free()
{
	__super::Free();
	Safe_Release(m_pBodyBottom);

}
#include "CTrigger_Box.h"
#include "CCollider_Base.h"
#include "CBoxColliderComponent.h"
#include "CBounding_AABB.h"	
#include "Client_Defines.h"

USING(Client)

CTrigger_Box::CTrigger_Box(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CGameObject(pDevice,pContext)
{
}

CTrigger_Box::CTrigger_Box(const CTrigger_Box& rhs)
	:CGameObject(rhs)
{
}

HRESULT CTrigger_Box::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTrigger_Box::Initialize_Copytype(void* pArg)
{
	if (FAILED(__super::Initialize_Copytype(pArg)))
		return E_FAIL;

	if(FAILED(Ready_Components(pArg)))
		return E_FAIL;

	return S_OK;
}

void CTrigger_Box::Update_Priority(_float fTimeDelta)
{
	__super::Update_Priority(fTimeDelta);
}

void CTrigger_Box::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CTrigger_Box::Update_Late(_float fTimeDelta)
{
	__super::Update_Late(fTimeDelta);
	if (pBoxCollider)
		pBoxCollider->Update_Collider(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));
}

void CTrigger_Box::Update_Render(_float fTimeDelta)
{
	__super::Update_Render(fTimeDelta);
#ifdef _DEBUG
	m_pGameInstance->Add_RenderObject(ENUM_TO_UINT(RENDERGROUP::NONALPHA),this);
#endif // _DEBUG

}

HRESULT CTrigger_Box::Render()
{
#ifdef _DEBUG
	if (pBoxCollider)
		pBoxCollider->Render();
#endif // _DEBUG


	return S_OK;
}

HRESULT CTrigger_Box::Ready_Components(void* pArg)
{
	TriggerBoxDesc* pTriggerDesc = static_cast<TriggerBoxDesc*>(pArg);
	CCollider_Base::COLLIDER_DESC ColDesc;
	ColDesc.m_eColGroup = ENUM_TO_UINT(COLLISION_GROUP::TRIGGER);

	CBounding_AABB::BOUNDING_AABB_DESC aabbDesc;
	aabbDesc.vCenter = pTriggerDesc->vCenter;
	aabbDesc.Extents = pTriggerDesc->vExtents;
	ColDesc.m_BoundingDesc = &aabbDesc;
	ColDesc.m_iLevelID = m_iSceneID;

	pBoxCollider= dynamic_cast<CBoxColliderComponent*>(m_pGameInstance->Clone_Prototype(
		PROTOTYPE::COMPONENT,
		0,
		PROTO_COMPONENT_NAME(L"BoxCollider"),
		&ColDesc)
		);
	
	CheckNullResult(pBoxCollider, E_FAIL);

	pBoxCollider->Set_Owner(this);

	return S_OK;
}


CTrigger_Box* CTrigger_Box::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContex)
{
	CTrigger_Box* pInstance = new CTrigger_Box(_pDevice, _pDeviceContex);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create :CTrigger_Box ");
		Safe_Release(pInstance);

	}



	return pInstance;
}

CGameObject* CTrigger_Box::Clone(void* pArg)
{
	CTrigger_Box* pInstance = new CTrigger_Box(*this);
	if (FAILED(pInstance->Initialize_Copytype(pArg)))
	{
		MSG_BOX("Failed to Cloned :CTrigger_Box ");
		Safe_Release(pInstance);

	}
	return pInstance;
}

void CTrigger_Box::Free()
{
	__super::Free();

	Safe_Release(pBoxCollider);
}

#include "CMapInteractObject.h"
#include "CModel.h"
#include "CCollider_Base.h"
#include "CBounding_AABB.h"

CMapInteractObject::CMapInteractObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CMapModel(pDevice,pContext)
{
}

CMapInteractObject::CMapInteractObject(const CMapInteractObject& rhs)
	: CMapModel(rhs)
{
}

HRESULT CMapInteractObject::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;


	return S_OK;
}

HRESULT CMapInteractObject::Initialize_Copytype(void* pArg)
{
	if (FAILED(__super::Initialize_Copytype(pArg)))
		return E_FAIL;

	MapInteraction_DESC* pDesc = static_cast<MapInteraction_DESC*>(pArg);
	m_eInteractionType = pDesc->eInteractionType;


	return S_OK;
}

void CMapInteractObject::Update_Priority(_float fTimeDelta)
{
	__super::Update_Priority(fTimeDelta);
}

void CMapInteractObject::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CMapInteractObject::Update_Late(_float fTimeDelta)
{
	__super::Update_Late(fTimeDelta);
}

void CMapInteractObject::Update_Render(_float fTimeDelta)
{
	__super::Update_Render(fTimeDelta);
}

HRESULT CMapInteractObject::Render()
{
	__super::Render();
	return S_OK;
}

void CMapInteractObject::Save_To_Json(json& Json)
{
	json Meta;
	wstring name = m_pModel->Get_ModelData().name;
	Meta["ModelName"] = WStringToUTF8(name);

	json Transform;
	//Transform정보
	_float3 s, r, t;


	XMStoreFloat3(&s, m_pTransformCom->Get_SRT(SRTType::SCALE));
	XMStoreFloat3(&t, m_pTransformCom->Get_SRT(SRTType::TRANSFORM));
	r = m_pTransformCom->Get_Rotation_ByEular();

	json position = json::array();
	position.push_back(t.x);
	position.push_back(t.y);
	position.push_back(t.z);
	Transform["Position"] = position;



	json scale = json::array();
	scale.push_back(s.x);
	scale.push_back(s.y);
	scale.push_back(s.z);
	Transform["Scale"] = scale;



	json Rotation = json::array();
	Rotation.push_back(r.x);
	Rotation.push_back(r.y);
	Rotation.push_back(r.z);
	Transform["Rotation"] = Rotation;

	Meta["Transform"] = Transform;

	//Collider정보
	json Collider;
	CBounding_AABB* pBounding = dynamic_cast<CBounding_AABB*>(pColliderComp->Get_Bounding());

	_float3 vCenter= pBounding->Get_OrignialDesc()->Center;
	_float3 vExtent= pBounding->Get_OrignialDesc()->Extents;
	Collider["Type"] = "AABB";


	json ColliderCenter = json::array();
	ColliderCenter.push_back(vCenter.x);
	ColliderCenter.push_back(vCenter.y);
	ColliderCenter.push_back(vCenter.z);
	Collider["Center"] = ColliderCenter;

	json ColliderExtent = json::array();
	ColliderExtent.push_back(vExtent.x);
	ColliderExtent.push_back(vExtent.y);
	ColliderExtent.push_back(vExtent.z);
	Collider["Extent"] = ColliderExtent;

	Meta["Collider"] = Collider;

	//Interaction정보
	string InteractionInfo = "";
	switch (InteractionType(m_eInteractionType))
	{
	case InteractionType::ROCK:
		InteractionInfo = "Rock";

		break;
	
	case InteractionType::LAWN:
		InteractionInfo = "Lawn";
		break;


	case InteractionType::GRASS:
		InteractionInfo = "Grass";
		break;


	}

	Meta["InteractionType"] = InteractionInfo;

	Json.push_back(Meta);
}

CMapInteractObject* CMapInteractObject::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
{
	CMapInteractObject* pInstance = new CMapInteractObject(_pDevice, _pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create :CMapInteractObject ");
		Safe_Release(pInstance);

	}

	return pInstance;
}

CGameObject* CMapInteractObject::Clone(void* pArg)
{
	CMapInteractObject* pInstance = new CMapInteractObject(*this);
	if (FAILED(pInstance->Initialize_Copytype(pArg)))
	{
		MSG_BOX("Failed to Cloned :CMapInteractObject ");
		Safe_Release(pInstance);

	}
	return pInstance;
}

void CMapInteractObject::Free()
{
	__super::Free();
}

#include "CMapObject.h"
#include "CGameInstance.h"
#include "CBoxColliderComponent.h"
#include "CMapObject_Manager.h"
#include "MathUtils.h"
#include "CShader.h"
#include "CCollider_Base.h"
#include "ImGuizmo.h"
#include "CInput_Manager.h"
#include "CMapLayer.h"


CMapObject::CMapObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CGameObject(pDevice,pContext),
	pColliderComp{ nullptr },
	m_pMapObject_Manager{ CMapObject_Manager ::GetInstance()},
	m_pInputManager{ CInput_Manager::GetInstance() }
{
	

}

CMapObject::CMapObject(const CMapObject& rhs)
	:CGameObject{rhs}
	,m_eRenderGroup{rhs.m_eRenderGroup},
	pColliderComp{nullptr},
	m_pMapObject_Manager{rhs.m_pMapObject_Manager},
	m_pInputManager{rhs.m_pInputManager}
{
	
}


HRESULT CMapObject::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMapObject::Initialize_Copytype(void* pArg)
{
	if(FAILED(__super::Initialize_Copytype(pArg)))
		return E_FAIL;


	if (FAILED(Ready_Component(pArg)))
		return E_FAIL;


	if (FAILED(Ready_Resource(pArg)))
		return E_FAIL;

	return S_OK;
}

void CMapObject::Update_Priority(_float fTimeDelta)
{
	__super::Update_Priority(fTimeDelta);
}

void CMapObject::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
	CheckNull(m_pTransformCom);
	CheckNull(pColliderComp);
		
	
	pColliderComp->Update_Collider(m_pTransformCom);

	if (m_bSelected)
		m_passName = "Select";

	else
		m_passName = "Default";
	

}

void CMapObject::Update_Late(_float fTimeDelta)
{
	__super::Update_Late(fTimeDelta);
}


void CMapObject::Update_Render(_float fTimeDelta)
{
	__super::Update_Render(fTimeDelta);
	m_pGameInstance->Add_RenderObject(m_eRenderGroup, this);

}

HRESULT CMapObject::Render()
{
	__super::Render();

	return S_OK;
}

bool CMapObject::Is_Picked(_vector Origin, _vector Dir, float& Dist)
{
	CheckNullResult(pColliderComp, false);

	bool Result= pColliderComp->Intersects_Ray(Origin, Dir, Dist);


	return Result;
}

HRESULT CMapObject::Ready_Component(void* pArg)
{

	MapObject_DESC* pDesc = static_cast<MapObject_DESC*>(pArg);

	//없다면 자동으로 만들어주자.
	CCollider_Base::COLLIDER_DESC ColDesc;
	CComponent::COMPONENT_DESC* ColliderDesc = static_cast<CCollider_Base::COLLIDER_DESC*>(pDesc->ColliderComponent);
	if (!ColliderDesc)
	{
		ColDesc.vScaleOffSet = { 0.3f,0.3f,0.3f };
		ColliderDesc = &ColDesc;

		pDesc->ColliderComponent = &ColDesc;

	}

	ColliderDesc->pOwner = this;

	CComponent* pCollider = dynamic_cast<CBoxColliderComponent*>(m_pGameInstance->Clone_Prototype
	(PROTOTYPE::COMPONENT, 0, PROTO_COMPONENT_NAME(L"BoxColliderComponent"), pDesc->ColliderComponent));

	if (FAILED(Add_Component(COMPONENT_TYPE::BOX_COLLIDER, pCollider, reinterpret_cast<CComponent**>(&pColliderComp))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMapObject::Ready_Resource(void* pArg)
{
	CheckNullResult(pArg, E_FAIL);

	MapObject_DESC* pDesc = static_cast<MapObject_DESC*>(pArg);

	m_eObjType = pDesc->ObjType;
	m_eRenderGroup = pDesc->eRenderGroup;
	m_ShaderName = pDesc->ShaderName;
	m_passName = pDesc->passName;

	m_pShader = m_pGameInstance->Find_Shader(m_ShaderName);
	Safe_AddRef(m_pShader);

	return S_OK;
}



void CMapObject::Free()
{
	__super::Free();

	Safe_Release(m_pShader);
	Safe_Release(pColliderComp);

}

void CMapObject::OnSeletected(bool bSelected)
{
	m_bSelected = bSelected;
	if (m_bSelected)
		m_passName = "Select";

	else
		m_passName = "Default";

}

void CMapObject::Save_To_Json(json& Json)
{
}

void CMapObject::Show_Gizmo()
{
	
	

	ImGuiIO& io = ImGui::GetIO();
	ImGuizmo::SetDrawlist();
	ImGuizmo::SetRect(0, 0, 1280,720);

	_float4x4 view = CGameInstance::GetInstance()->Get_ViewMatrix(ENUM_TO_UINT(CAMERA_TYPE::FREE));
	_float4x4 proj = CGameInstance::GetInstance()->Get_ProjMatrix(ENUM_TO_UINT(CAMERA_TYPE::FREE));

	_float4x4 Wolrd = m_pTransformCom->Get_World(TransformScope::WORLD);


	static ImGuizmo::OPERATION op = ImGuizmo::TRANSLATE;
	static ImGuizmo::MODE mode = ImGuizmo::WORLD;

	//Gizmo 호출
	ImGuizmo::Manipulate((float*)&view,
		(float*)&proj,
		op, mode,
		(float*)&Wolrd);



}

CGameObject* CMapObject::Clone(void* pArg)
{
	return nullptr;
}



void CMapObject::Imgui_Render_Properties(_float3* vScale, _float3* vPosition, _float3* vRotation)
{

	ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "Name : %s", WStringToUTF8(tag).c_str());

	string Type = "";
	switch (m_eObjType)
	{
	case MapObjType::OBSTACLE:
		Type = "OBSTACLE";
		break;

	case MapObjType::TILE:
		Type = "TILE";
		break;

	case MapObjType::TERRAIN:
		Type = "TERRAIN";
		break;
	case MapObjType::POSITION:
		Type = "POSITION";
		break;

	case MapObjType::TRIGGER:
		Type = "TRIGGER";
		break;

	default:
		break;
	}
	ImGui::TextColored(ImVec4(0.f, 1.f, 0.f, 1.f), "Type: %s", Type.c_str());

	(*vScale).x = m_pTransformCom->Get_Scale_ByFloat3().x;
	(*vScale).y = m_pTransformCom->Get_Scale_ByFloat3().y;
	(*vScale).z = m_pTransformCom->Get_Scale_ByFloat3().z;

	XMStoreFloat3(vPosition, m_pTransformCom->Get_State(STATE::POSITION));

	*vRotation = MathUtils::QuaternionToEuler(m_pTransformCom->Get_SRT(SRTType::ROTATION));

}

void CMapObject::Edit_Move(DIRECTION eDir, float fSpeed, float _fTimeDelta)
{
	m_pTransformCom->Set_Speed(fSpeed);

	switch (eDir)
	{
	case Engine::DIRECTION::FORWARD:
	case Engine::DIRECTION::RIGHT:
	case Engine::DIRECTION::LEFT:
	case Engine::DIRECTION::BACKWARD:
		m_pTransformCom->Move(eDir, _fTimeDelta);
		break;

	default:
		break;
	}
}

void CMapObject::Fix_Y(_float Y)
{
}

void CMapObject::Update_SelectMode(float _fTimeDelta)
{
	float m_fMoveSpeed = CMapObject_Manager::GetInstance()->Get_MoveSpeed();

	
	if (m_pInputManager->IsKeyPressed(KeyCode::Delete))
	{
		
		CMapLayer* pTargetLayer = m_pMapObject_Manager->Get_Layer_By_MapObjType(m_eObjType);
		if (pTargetLayer)
			pTargetLayer->RequestDestroy(this);

		CMapObject_Manager::GetInstance()->Set_SelectObject(nullptr);
	}

	if (m_pInputManager->IsKeyHeld(KeyCode::LShift))
		Fix_Y(0.f);


	else if (m_pInputManager->IsKeyHeld(KeyCode::LControl))
	{
		if (m_pInputManager->IsKeyHeld(KeyCode::UpArrow))
			Edit_Move(DIRECTION::UP, m_fMoveSpeed, _fTimeDelta);

		else if (m_pInputManager->IsKeyHeld(KeyCode::DownArrow))
			Edit_Move(DIRECTION::DOWN, m_fMoveSpeed, _fTimeDelta);
	}

	else
	{
		if (m_pInputManager->IsKeyHeld(KeyCode::UpArrow))
			Edit_Move(DIRECTION::FORWARD, m_fMoveSpeed, _fTimeDelta);

		else if (m_pInputManager->IsKeyHeld(KeyCode::DownArrow))
			Edit_Move(DIRECTION::BACKWARD, m_fMoveSpeed, _fTimeDelta);

		else if (m_pInputManager->IsKeyHeld(KeyCode::LeftArrow))
			Edit_Move(DIRECTION::LEFT, m_fMoveSpeed, _fTimeDelta);

		else if (m_pInputManager->IsKeyHeld(KeyCode::RightArrow))
			Edit_Move(DIRECTION::RIGHT, m_fMoveSpeed, _fTimeDelta);

	}
}


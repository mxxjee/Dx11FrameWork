#include "CMapObject.h"
#include "CGameInstance.h"
#include "CBoxColliderComponent.h"
#include "CMapObject_Manager.h"
#include "MathUtils.h"
#include "CShader.h"
#include "CCollider_Base.h"


CMapObject::CMapObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CGameObject{ pDevice,pContext },
	m_eObjType{ MapObjType::OBSTACLE },
	pColliderComp{ nullptr },
	m_pMapObject_Manager{ CMapObject_Manager ::GetInstance()}
{
	

}

CMapObject::CMapObject(const CMapObject& rhs)
	:CGameObject{rhs}
	,m_bSelected{rhs.m_bSelected}
	,m_eRenderGroup{rhs.m_eRenderGroup},
	pColliderComp{nullptr},
	m_pMapObject_Manager{rhs.m_pMapObject_Manager}
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

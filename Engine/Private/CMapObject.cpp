#include "CMapObject.h"
#include "CGameInstance.h"
#include "CSphereColliderComponent.h"
#include "CMapObject_Manager.h"
#include "MathUtils.h"
#include	"CShader.h"

CMapObject::CMapObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CGameObject{ pDevice,pContext },
	m_eObjType{ MapObjType::MODEL },
	pColliderComp{ nullptr },
	m_pMapObject_Manager{ CMapObject_Manager ::GetInstance()}
{
	Safe_AddRef(m_pMapObject_Manager);

}

CMapObject::CMapObject(const CMapObject& rhs)
	:CGameObject{rhs}
	,m_bSelected{rhs.m_bSelected}
	,m_eRenderGroup{rhs.m_eRenderGroup},
	pColliderComp{nullptr},
	m_pMapObject_Manager{rhs.m_pMapObject_Manager}
{
	Safe_AddRef(m_pMapObject_Manager);
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
		
	
	pColliderComp->UpdateCollider(m_pTransformCom);

	

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

bool CMapObject::Is_Picked(HWND hWnd, _float4x4& Proj, _float4x4& View, float& Dist)
{
	CheckNullResult(pColliderComp, false);

	Ray ray = MathUtils::CreateRayWorld(hWnd,m_pContext,this, Proj,View);
	bool Result= pColliderComp->Intersects(ray.Origin,ray.Dir, Dist);
	m_pMapObject_Manager->Set_SelectObject(this);

	return Result;
}

HRESULT CMapObject::Ready_Component(void* pArg)
{

	MapObject_DESC* pDesc = static_cast<MapObject_DESC*>(pArg);

	//없다면 자동으로 만들어주자.
	CSphereColliderComponent::BOUNDINGSPHERE_DESC ColDesc;
	CComponent::COMPONENT_DESC* ColliderDesc = static_cast<CSphereColliderComponent::BOUNDINGSPHERE_DESC*>(pDesc->SphereColliderComponent);
	if (!ColliderDesc)
	{
		ColDesc.Center = _float3(0.f, 0.f, 0.f);
		ColDesc.Radius = 1.f;
		ColliderDesc = &ColDesc;
		pDesc->SphereColliderComponent = &ColliderDesc;
		

	}
	ColliderDesc->pOwner = this;

	CComponent* pCollider = dynamic_cast<CSphereColliderComponent*>(m_pGameInstance->Clone_Prototype
	(PROTOTYPE::COMPONENT, 0, PROTO_COMPONENT_NAME(L"SphereColliderComponent"), pDesc->SphereColliderComponent));

	if (FAILED(Add_Component(COMPONENT_TYPE::SPHERE_COLLIDER, pCollider, reinterpret_cast<CComponent**>(&pColliderComp))))
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
	Safe_AddRef(m_pMapObject_Manager);
	Safe_Release(m_pShader);
	Safe_Release(pColliderComp);

}

#include "CMapTrigger.h"
#include "CCollider_Base.h"
#include "CTransform.h"
#include "CBoxColliderComponent.h"
#include "CBounding_AABB.h"
#include "CGameInstance.h"


CMapTrigger::CMapTrigger(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CMapObject(pDevice,pContext)
{
}

CMapTrigger::CMapTrigger(const CMapObject& rhs)
    : CMapObject(rhs)
{
}

HRESULT CMapTrigger::Initialize_Prototype()
{
  

    return S_OK;
}

HRESULT CMapTrigger::Initialize_Copytype(void* pArg)
{
    if (FAILED(__super::Initialize_Copytype(pArg)))
        return E_FAIL;



    return S_OK;
}

void CMapTrigger::Update_Priority(_float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);

}

void CMapTrigger::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CMapTrigger::Update_Late(_float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);
    pColliderComp->Update_Collider(XMLoadFloat4x4(&m_pTransformCom->Get_World()));

}

void CMapTrigger::Update_Render(_float fTimeDelta)
{
    __super::Update_Render(fTimeDelta);
}


HRESULT CMapTrigger::Render()
{
#ifdef _DEBUG
    if (CGameInstance::m_bDrawDebug)
    {
        if (pColliderComp)
            pColliderComp->Render();
    }
#endif
    return S_OK;
}

CMapTrigger* CMapTrigger::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
{

    CMapTrigger* pInstance = new CMapTrigger(_pDevice, _pDeviceContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create :CMapTrigger ");
        Safe_Release(pInstance);

    }



    return pInstance;
}

CGameObject* CMapTrigger::Clone(void* pArg)
{
    CMapTrigger* pInstance = new CMapTrigger(*this);
    if (FAILED(pInstance->Initialize_Copytype(pArg)))
    {
        MSG_BOX("Failed to Cloned :CMapTrigger ");
        Safe_Release(pInstance);

    }
    return pInstance;
}

void CMapTrigger::Free()
{
    __super::Free();
    Safe_Release(pColliderComp);
}

bool CMapTrigger::Is_Picked(_vector Origin, _vector Dir, float& Dist)
{
    return pColliderComp->Intersects_Ray(Origin, Dir, Dist, m_pTransformCom);

}



#ifdef _DEBUG
void CMapTrigger::Imgui_Render_Properties(_float3* vScale, _float3* vPosition, _float3* vRotation)
{
    __super::Imgui_Render_Properties(vScale, vPosition, vRotation);
    
}
#endif
HRESULT CMapTrigger::Ready_Component(void* pArg)
{
    MapObject_DESC* Desc = static_cast<MapObject_DESC*>(pArg);

    if (pArg == nullptr || !Desc->ColliderComponent)
    {
        CBoxColliderComponent::COLLIDER_DESC ColDesc;
        CBounding_AABB::BOUNDING_AABB_DESC AABBDEsc;

        AABBDEsc.Extents = { 0.1f,0.1f,0.1f };

        ColDesc.m_BoundingDesc = &AABBDEsc;
        ColDesc.pOwner = this;


        CComponent* pBoxCollider = dynamic_cast<CBoxColliderComponent*>(m_pGameInstance->Clone_Prototype(
            PROTOTYPE::COMPONENT,
            0,
            PROTO_COMPONENT_NAME(L"BoxColliderComponent"),
            &ColDesc));

        if (FAILED(Add_Component(
            COMPONENT_TYPE::BOX_COLLIDER,
            pBoxCollider,
            reinterpret_cast<CComponent**>(&pColliderComp)
        )))
            return E_FAIL;

    }

    else
    {
        MapObject_DESC* pDesc = static_cast<MapObject_DESC*>(pArg);
        CBoxColliderComponent::COLLIDER_DESC* ColDesc=static_cast<CBoxColliderComponent::COLLIDER_DESC*>(pDesc->ColliderComponent);
        CComponent* pBoxCollider = dynamic_cast<CBoxColliderComponent*>(m_pGameInstance->Clone_Prototype(
            PROTOTYPE::COMPONENT,
            0,
            PROTO_COMPONENT_NAME(L"BoxColliderComponent"),
            pDesc->ColliderComponent));

        if (FAILED(Add_Component(
            COMPONENT_TYPE::BOX_COLLIDER,
            pBoxCollider,
            reinterpret_cast<CComponent**>(&pColliderComp)
        )))
            return E_FAIL;
    }
   

    return S_OK;
}

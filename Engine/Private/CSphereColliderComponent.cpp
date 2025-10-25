#include "CSphereColliderComponent.h"
#include "CTransform.h"
CSphereColliderComponent::CSphereColliderComponent(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CComponent(pDevice,pContext)
{
}

CSphereColliderComponent::CSphereColliderComponent(const CSphereColliderComponent& Prototype)
    :CComponent(Prototype), m_Sphere(Prototype.m_Sphere)
{
}

HRESULT CSphereColliderComponent::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CSphereColliderComponent::Initialize_Copytype(void* pArg)
{
    BOUNDINGSPHERE_DESC* pDesc = static_cast<BOUNDINGSPHERE_DESC*>(pArg);

    m_Sphere.Center = pDesc->Center;
    m_Sphere.Radius = pDesc->Radius;

    return S_OK;
}

HRESULT CSphereColliderComponent::UpdateCollider(CTransform* pTransform)
{
    CheckNullResult(pTransform, E_FAIL);


    XMStoreFloat3(&m_Sphere.Center, pTransform->Get_State(STATE::POSITION, TransformScope::WORLD));
    
    _float3 vScale = pTransform->Get_Scale_ByFloat3();
    m_Sphere.Radius=max(vScale.x, vScale.y,vScale.z);

    return S_OK;
}

bool CSphereColliderComponent::Intersects(_vector origin, _vector rayDir, _float& Dist)
{
    return m_Sphere.Intersects(origin, rayDir, Dist);
}

CSphereColliderComponent* CSphereColliderComponent::Clone(void* pArg)
{
    CSphereColliderComponent* pInstance = new CSphereColliderComponent(*this);
    if (FAILED(pInstance->Initialize_Copytype(pArg)))
    {
        MSG_BOX("Failed to Cloned : CSphereColliderComponent");
        Safe_Release(pInstance);
    }


    return pInstance;
}


CSphereColliderComponent* CSphereColliderComponent::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
    CSphereColliderComponent* pInstance = new CSphereColliderComponent(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CSphereColliderComponent");
        Safe_Release(pInstance);
    }


    return pInstance;
}

void CSphereColliderComponent::Free()
{
    __super::Free();
}

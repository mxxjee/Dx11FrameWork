#include "CSphereColliderComponent.h"
#include "CTransform.h"
CSphereColliderComponent::CSphereColliderComponent(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CCollider_Base(pDevice,pContext)
{
}

CSphereColliderComponent::CSphereColliderComponent(const CSphereColliderComponent& Prototype)
    : CCollider_Base(Prototype), m_Sphere(Prototype.m_Sphere)
{
}

HRESULT CSphereColliderComponent::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CSphereColliderComponent::Initialize_Copytype(void* pArg)
{
   if(FAILED(__super::Initialize_Copytype(pArg)))
       return E_FAIL;

   m_eType = COLLIDER_TYPE::SPHERE;


    return S_OK;
}

HRESULT CSphereColliderComponent::Update_Collider(CTransform* pTransform)
{

    XMStoreFloat3(&vCenter, pTransform->Get_State(STATE::POSITION));
    XMStoreFloat3(&m_Sphere.Center, XMLoadFloat3(&vOffset) + XMLoadFloat3(&vCenter));


    _float3 vScale = pTransform->Get_Scale_ByFloat3();
    float Radius= max(vScale.x, vScale.y);
    Radius = max(Radius, vScale.z);

    m_Sphere.Radius = Radius * vScaleOffSet.x;


    return S_OK;
}

bool CSphereColliderComponent::Intersects_Ray(_vector origin, _vector rayDir, _float& Dist)
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

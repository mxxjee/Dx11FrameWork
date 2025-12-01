#include "CSphereColliderComponent.h"
#include "CTransform.h"
#include "CBounding_Sphere.h"


CSphereColliderComponent::CSphereColliderComponent(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CCollider_Base(pDevice,pContext)
{
}

CSphereColliderComponent::CSphereColliderComponent(const CSphereColliderComponent& Prototype)
    : CCollider_Base(Prototype)
{
}

HRESULT CSphereColliderComponent::Initialize_Prototype()
{
    if (__super::Initialize_Prototype())
        return E_FAIL;


    m_eType = COLLIDER_TYPE::SPHERE;

    return S_OK;
}

HRESULT CSphereColliderComponent::Initialize_Copytype(void* pArg)
{
    
    if (FAILED(__super::Initialize_Copytype(pArg)))
        return E_FAIL;

    COLLIDER_DESC* pDesc = static_cast<COLLIDER_DESC*>(pArg);
    CBounding::BOUNDING_DESC* pBoundingDesc = static_cast<CBounding::BOUNDING_DESC*>(pDesc->m_BoundingDesc);

    m_pBounding = CBounding_Sphere::Create(m_pDevice, m_pContext, pBoundingDesc);


    return S_OK;
}

HRESULT CSphereColliderComponent::Update_Collider(XMMATRIX    WorldMatrix)
{
    
    m_pBounding->Update(WorldMatrix);

    return S_OK;
}

bool CSphereColliderComponent::Intersects_Ray(_vector origin, _vector rayDir, _float& Dist, CTransform* pTransform)
{
    return m_pBounding->Intersects_Ray(origin, rayDir, Dist);

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

void CSphereColliderComponent::Set_Radius(_float fRadius)
{
    dynamic_cast<CBounding_Sphere*>(m_pBounding)->Set_Raidus(fRadius);
}



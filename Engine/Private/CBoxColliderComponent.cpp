#include "CBoxColliderComponent.h"
#include "CTransform.h"
#include "CBounding_AABB.h"

CBoxColliderComponent::CBoxColliderComponent(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CCollider_Base(pDevice, pContext)
{
}

CBoxColliderComponent::CBoxColliderComponent(const CBoxColliderComponent& Prototype)
    : CCollider_Base(Prototype)
{
}

HRESULT CBoxColliderComponent::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;


    m_eType = COLLIDER_TYPE::AABB;

    return S_OK;
}

HRESULT CBoxColliderComponent::Initialize_Copytype(void* pArg)
{
    
    if (FAILED(__super::Initialize_Copytype(pArg)))
        return E_FAIL;

    COLLIDER_DESC* pDesc = static_cast<COLLIDER_DESC*>(pArg);
    CBounding::BOUNDING_DESC* pBoundingDesc = static_cast<CBounding::BOUNDING_DESC*>(pDesc->m_BoundingDesc);

    m_pBounding = CBounding_AABB::Create(m_pDevice, m_pContext, pBoundingDesc);


    return S_OK;
}

HRESULT CBoxColliderComponent::Update_Collider(XMMATRIX    WorldMatrix)
{
    m_pBounding->Update(WorldMatrix);

    return S_OK;
}

bool CBoxColliderComponent::Intersects_Ray(_vector origin, _vector rayDir, _float& Dist, CTransform* pTransform)
{
    return m_pBounding->Intersects_Ray(origin, rayDir, Dist);

}

bool CBoxColliderComponent::Push_Collision(CCollider_Base* pOther, _float3& vOutPush)
{
    COLLIDER_TYPE eOtherType = pOther->Get_Type();
    CBounding_AABB* pAABB = static_cast<CBounding_AABB*>(m_pBounding);
    bool bResult = false;

    switch (eOtherType)
    {
    case Engine::COLLIDER_TYPE::AABB:
        bResult= pAABB->Compute_PushOut_AABBAABB(pOther->Get_Bounding(), vOutPush);
        break;

    default:
        break;
    }


    return bResult;
}


void CBoxColliderComponent::Set_OffSet(_float3 vOffset)
{
    return dynamic_cast<CBounding_AABB*>(m_pBounding)->Set_OffSet(vOffset);
}

void CBoxColliderComponent::Set_Size(_float3 vSize)
{
    return dynamic_cast<CBounding_AABB*>(m_pBounding)->Set_Size(vSize);
}



CBoxColliderComponent* CBoxColliderComponent::Clone(void* pArg)
{
    CBoxColliderComponent* pInstance = new CBoxColliderComponent(*this);
    if (FAILED(pInstance->Initialize_Copytype(pArg)))
    {
        MSG_BOX("Failed to Cloned : CBoxColliderComponent");
        Safe_Release(pInstance);
    }


    return pInstance;
}

void CBoxColliderComponent::Free()
{
    __super::Free();
}

CBoxColliderComponent* CBoxColliderComponent::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
    CBoxColliderComponent* pInstance = new CBoxColliderComponent(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CBoxColliderComponent");
        Safe_Release(pInstance);
    }


    return pInstance;
}

#include "COBBColliderComponent.h"
#include "CTransform.h"
#include "CBounding_OBB.h"

COBBColliderComponent::COBBColliderComponent(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CCollider_Base(pDevice, pContext)
{
}

COBBColliderComponent::COBBColliderComponent(const COBBColliderComponent& Prototype)
    : CCollider_Base(Prototype)
{
}

HRESULT COBBColliderComponent::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;


    m_eType = COLLIDER_TYPE::OBB;

    return S_OK;;
}

HRESULT COBBColliderComponent::Initialize_Copytype(void* pArg)
{
    
    if (FAILED(__super::Initialize_Copytype(pArg)))
        return E_FAIL;

    COLLIDER_DESC* pDesc = static_cast<COLLIDER_DESC*>(pArg);
    CBounding::BOUNDING_DESC* pBoundingDesc = static_cast<CBounding::BOUNDING_DESC*>(pDesc->m_BoundingDesc);

    m_pBounding = CBounding_OBB::Create(m_pDevice, m_pContext, pBoundingDesc);


    return S_OK;
}

HRESULT COBBColliderComponent::Update_Collider(XMMATRIX WorldMatrix)
{
    m_pBounding->Update(WorldMatrix);

    return S_OK;
}

bool COBBColliderComponent::Intersects_Ray(_vector origin, _vector rayDir, _float& Dist, CTransform* pTransform)
{
    return m_pBounding->Intersects_Ray(origin, rayDir, Dist);

}

void COBBColliderComponent::Set_OffSet(_float3 vOffset)
{
    return dynamic_cast<CBounding_OBB*>(m_pBounding)->Set_OffSet(vOffset);

}

void COBBColliderComponent::Set_Size(_float3 vSize)
{
    return dynamic_cast<CBounding_OBB*>(m_pBounding)->Set_Size(vSize);
}

COBBColliderComponent* COBBColliderComponent::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
    COBBColliderComponent* pInstance = new COBBColliderComponent(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : COBBColliderComponent");
        Safe_Release(pInstance);
    }


    return pInstance;
}
COBBColliderComponent* COBBColliderComponent::Clone(void* pArg)
{
    COBBColliderComponent* pInstance = new COBBColliderComponent(*this);
    if (FAILED(pInstance->Initialize_Copytype(pArg)))
    {
        MSG_BOX("Failed to Cloned : COBBColliderComponent");
        Safe_Release(pInstance);
    }


    return pInstance;
}

void COBBColliderComponent::Free()
{
    __super::Free();
}


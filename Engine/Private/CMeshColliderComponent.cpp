#include "CMeshColliderComponent.h"
#include "CTransform.h"
#include "CGameInstance.h"
#include "CModel.h"
#include "CMeshComponent.h"

#include "CBounding_Mesh.h"
#include "CBounding_AABB.h"

CMeshColliderComponent::CMeshColliderComponent(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CCollider_Base(pDevice,pContext), m_pGameInstance(CGameInstance::GetInstance())
{
    Safe_AddRef(m_pGameInstance);
}

CMeshColliderComponent::CMeshColliderComponent(const CMeshColliderComponent& Prototype)
    : CCollider_Base(Prototype),
    m_pMeshBounding(Prototype.m_pMeshBounding),
    m_pGameInstance(Prototype.m_pGameInstance)
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CMeshColliderComponent::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    m_EngineDesc = m_pGameInstance->Get_EngineDesc();
    m_eType = COLLIDER_TYPE::MESH;

    return S_OK;
}

HRESULT CMeshColliderComponent::Initialize_Copytype(void* pArg)
{

    if (FAILED(__super::Initialize_Copytype(pArg)))
        return E_FAIL;

    /*MeshCollider->AABB + MEsh*/
    COLLIDER_DESC* pDesc = static_cast<COLLIDER_DESC*>(pArg);
    CBounding::BOUNDING_DESC* pBoundingDesc = static_cast<CBounding::BOUNDING_DESC*>(pDesc->m_BoundingDesc);

    m_pBounding = CBounding_AABB::Create(m_pDevice, m_pContext, pBoundingDesc);
    m_pMeshBounding = CBounding_Mesh::Create(m_pDevice, m_pContext, pBoundingDesc);


    return S_OK;
}

HRESULT CMeshColliderComponent::Update_Collider(XMMATRIX    WorldMatrix)
{

   // _float4x4 WorldMatrix = pTransform->Get_World();
    m_pBounding->Update(WorldMatrix);
    m_pMeshBounding->Update(WorldMatrix);

    return S_OK;
}

bool CMeshColliderComponent::Intersects_Ray(_vector origin, _vector rayDir, _float& Dist,CTransform* pTransform)
{

    if (!m_pBounding->Intersects_Ray(origin, rayDir, Dist))
        return false;



    _matrix InvWorld = pTransform->Get_WorldInverse();

    _vector localOrigin = XMVector3TransformCoord(origin, InvWorld);
    _vector localDir = XMVector3TransformNormal(rayDir, InvWorld);
    localDir = XMVector3Normalize(localDir);


    return m_pMeshBounding->Intersects_Ray(localOrigin, localDir, Dist);
    


    
}

CMeshColliderComponent* CMeshColliderComponent::Clone(void* pArg)
{
    CMeshColliderComponent* pInstance = new CMeshColliderComponent(*this);
    if (FAILED(pInstance->Initialize_Copytype(pArg)))
    {
        MSG_BOX("Failed to Cloned : CMeshColliderComponent");
        Safe_Release(pInstance);
    }


    return pInstance;
}

void CMeshColliderComponent::Free()
{
    __super::Free();
    Safe_Release(m_pMeshBounding);
    Safe_Release(m_pGameInstance);


}

CMeshColliderComponent* CMeshColliderComponent::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
    CMeshColliderComponent* pInstance = new CMeshColliderComponent(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CMeshColliderComponent");
        Safe_Release(pInstance);
    }


    return pInstance;
}

bool CMeshColliderComponent::Intersect(CCollider_Base* pOther)
{
    m_isColl = m_pBounding->Intersect(pOther->Get_Type(), pOther->Get_Bounding());
    return m_isColl;
}

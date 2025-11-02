#include "CMeshColliderComponent.h"
#include "CTransform.h"
#include "CGameInstance.h"
#include "MathUtils.h"
#include "CModel.h"
#include "CMeshComponent.h"

CMeshColliderComponent::CMeshColliderComponent(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CCollider_Base(pDevice,pContext), m_pGameInstance(CGameInstance::GetInstance())
{
    Safe_AddRef(m_pGameInstance);
}

CMeshColliderComponent::CMeshColliderComponent(const CMeshColliderComponent& Prototype)
    : CCollider_Base(Prototype), 
    m_BoundingBox(Prototype.m_BoundingBox), 
    m_pGameInstance(Prototype.m_pGameInstance)
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CMeshColliderComponent::Initialize_Prototype()
{
    m_EngineDesc = m_pGameInstance->Get_EngineDesc();

    return S_OK;
}

HRESULT CMeshColliderComponent::Initialize_Copytype(void* pArg)
{
    if (FAILED(__super::Initialize_Copytype(pArg)))
        return E_FAIL;

    m_eType = COLLIDER_TYPE::MESH;

    CheckNullResult(pArg,E_FAIL);
    COLLIDER_MESH* pDesc = static_cast<COLLIDER_MESH*>(pArg);
    m_pModel = pDesc->pModel;

    return S_OK;
}

HRESULT CMeshColliderComponent::Update_Collider(CTransform* pTransform)
{
    XMStoreFloat3(&vCenter, pTransform->Get_State(STATE::POSITION));
    XMStoreFloat3(&m_BoundingBox.Center, XMLoadFloat3(&vOffset) + XMLoadFloat3(&vCenter));


    _float3 ObjScale = pTransform->Get_Scale_ByFloat3();

    XMStoreFloat3(&m_BoundingBox.Extents,
        XMVectorMultiply(XMLoadFloat3(&ObjScale),
            XMLoadFloat3(&vScaleOffSet)));

    return S_OK;
}

bool CMeshColliderComponent::Intersects_Ray(_vector origin, _vector rayDir, _float& Dist)
{
    CheckNullResult(m_pOwner,false);
    CheckNullResult(m_pModel,false);

    if (!m_BoundingBox.Intersects(origin, rayDir, Dist))
        return false;

    return m_pModel->Intersects_Ray(origin, rayDir, Dist);
    


    
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

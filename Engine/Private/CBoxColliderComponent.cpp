#include "CBoxColliderComponent.h"
#include "CTransform.h"
CBoxColliderComponent::CBoxColliderComponent(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CCollider_Base(pDevice, pContext)
{
}

CBoxColliderComponent::CBoxColliderComponent(const CBoxColliderComponent& Prototype)
    : CCollider_Base(Prototype), m_BoundingBox(Prototype.m_BoundingBox)
{
}

HRESULT CBoxColliderComponent::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBoxColliderComponent::Initialize_Copytype(void* pArg)
{
    if (FAILED(__super::Initialize_Copytype(pArg)))
        return E_FAIL;

    m_eType = COLLIDER_TYPE::AABB;

    return S_OK;
}

HRESULT CBoxColliderComponent::Update_Collider(CTransform* pTransform)
{

    
    XMStoreFloat3(&vCenter, pTransform->Get_State(STATE::POSITION));
    XMStoreFloat3(&m_BoundingBox.Center, XMLoadFloat3(&vOffset) + XMLoadFloat3(&vCenter));

    
    _float3 ObjScale = pTransform->Get_Scale_ByFloat3();

    XMStoreFloat3(&m_BoundingBox.Extents,
        XMVectorMultiply(XMLoadFloat3(&ObjScale),
        XMLoadFloat3(&vScaleOffSet)));


    return S_OK;
}

bool CBoxColliderComponent::Intersects_Ray(_vector origin, _vector rayDir, _float& Dist)
{
    return m_BoundingBox.Intersects(origin, rayDir, Dist);

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

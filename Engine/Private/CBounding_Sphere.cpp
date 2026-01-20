#include "CBounding_Sphere.h"
#include "CBounding_AABB.h"
#include "CBounding_OBB.h"


#include "DebugDraw.h"
#include "CTransform.h"

CBounding_Sphere::CBounding_Sphere(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CBounding(pDevice, pContext)
{
}


HRESULT CBounding_Sphere::Initialize(const BOUNDING_DESC* pInitialDesc)
{
    const BOUNDING_SPHERE_DESC* pDesc = static_cast<const BOUNDING_SPHERE_DESC*> (pInitialDesc);

    m_pOriginalDesc = new BoundingSphere(pDesc->vCenter, pDesc->fRadius);
    m_pDesc = new BoundingSphere(*m_pOriginalDesc);

    return S_OK;
}

HRESULT CBounding_Sphere::Update(XMMATRIX WorldMatrix)
{
    //월드행렬을 따라가되, 내가 설정한 콜라이더자체의 offset과 scale을 포함한다.
    _matrix NewWorld = XMMatrixMultiply(WorldMatrix, XMMatrixScaling(m_fRadius, m_fRadius, m_fRadius));


    m_pOriginalDesc->Transform((*m_pDesc), NewWorld);



    return S_OK;
}

bool CBounding_Sphere::Intersects_Ray(_vector origin, _vector rayDir, _float& Dist)
{
    return m_pDesc->Intersects(origin, rayDir, Dist);

}

bool CBounding_Sphere::Compute_PushOut_SphereSphere(CBounding* pOther, _float3& vOut)
{
    CBounding_Sphere *pOtherBound = static_cast<CBounding_Sphere*>(pOther);
    BoundingSphere* pOtherDesc = pOtherBound->Get_Desc();

    _vector cA = XMLoadFloat3(&m_pDesc->Center);
    _vector cB = XMLoadFloat3(&pOtherDesc->Center);

    _vector delta = cA - cB;
    float dist = XMVectorGetX(XMVector3Length(delta));
    float minDist = m_pDesc->Radius + pOtherDesc->Radius;

    if (dist >= minDist || dist == 0.0f)
        return false;
  
    float fOverlap = minDist - dist;
    XMVECTOR dir = XMVector3Normalize(delta);

    XMVECTOR push = dir * fOverlap;
    XMStoreFloat3(&vOut, push);
    return true;
}

#ifdef _DEBUG

HRESULT CBounding_Sphere::Render(PrimitiveBatch<VertexPositionColor>* pBatch, _bool isColl)
{
    DX::Draw(pBatch, *m_pDesc, isColl == true ? XMVectorSet(1.f, 0.f, 0.f, 1.f) : XMVectorSet(0.f, 1.f, 0.f, 1.f));

    return E_NOTIMPL;
}
#endif // DEBUG

CBounding_Sphere* CBounding_Sphere::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const BOUNDING_DESC* pInitialDesc)
{
    CBounding_Sphere* pInstance = new CBounding_Sphere(pDevice, pContext);

    if (FAILED(pInstance->Initialize(pInitialDesc)))
    {
        MSG_BOX("Failed to Created : CBounCBounding_Sphereding_AABB");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CBounding_Sphere::Free()
{
    __super::Free();

    Safe_Delete(m_pOriginalDesc);
    Safe_Delete(m_pDesc);

}

_float3 CBounding_Sphere::Get_MaxBound(_vector vCenter)
{
    _float3 fCenter;
    XMStoreFloat3(&fCenter, vCenter);

   
    return _float3(m_pDesc->Center.x + m_pDesc->Radius,
                    m_pDesc->Center.y + m_pDesc->Radius,
                    m_pDesc->Center.z + m_pDesc->Radius);

}

_float3 CBounding_Sphere::Get_MinBound(_vector vCenter)
{
    _float3 fCenter;
    XMStoreFloat3(&fCenter, vCenter);

    return _float3(m_pDesc->Center.x - m_pDesc->Radius,
        m_pDesc->Center.y - m_pDesc->Radius,
        m_pDesc->Center.z - m_pDesc->Radius);
}

bool CBounding_Sphere::Intersect(COLLIDER_TYPE eType, CBounding* pOther)
{
    switch (eType)
    {
    case COLLIDER_TYPE::AABB:
        return m_pDesc->Intersects(*static_cast<CBounding_AABB*>(pOther)->Get_Desc());
        break;

    case COLLIDER_TYPE::OBB:
        return m_pDesc->Intersects(*static_cast<CBounding_OBB*>(pOther)->Get_Desc());
        break;

    case COLLIDER_TYPE::SPHERE:
        return m_pDesc->Intersects(*static_cast<CBounding_Sphere*>(pOther)->Get_Desc());
        break;
    }

    return false;
}

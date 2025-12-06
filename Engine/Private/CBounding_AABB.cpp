#include "CBounding_AABB.h"
#include "CBounding_OBB.h"
#include "CBounding_Sphere.h"
#include "DebugDraw.h"
#include "CTransform.h"


CBounding_AABB::CBounding_AABB(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CBounding(pDevice,pContext)
{
}

HRESULT CBounding_AABB::Initialize(const BOUNDING_DESC* pInitialDesc)
{
    const BOUNDING_AABB_DESC* pDesc = static_cast<const BOUNDING_AABB_DESC*> (pInitialDesc);
    

    _float3 vNewCenter;
    XMStoreFloat3(&vNewCenter, XMLoadFloat3(&pDesc->vCenter));

    m_pOriginalDesc = new BoundingBox(vNewCenter, pDesc->Extents);
    m_pDesc = new BoundingBox(*m_pOriginalDesc);

    return S_OK;
}

HRESULT CBounding_AABB::Update(XMMATRIX WorldMatrix)
{
      //월드행렬을 따라가되, 내가 설정한 콜라이더자체의 offset과 scale을 포함한다.
    XMMATRIX NewWorld = XMMatrixScaling(m_vSize.x, m_vSize.y, m_vSize.z) * XMMatrixTranslation(m_vOffSet.x, m_vOffSet.y, m_vOffSet.z);

    XMMATRIX Final = NewWorld * WorldMatrix;

    //회전값 지우기
    Final.r[0] = XMVectorSet(1.f, 0.f, 0.f, 0.f) * XMVector3Length(Final.r[0]);
    Final.r[1] = XMVectorSet(0.f, 1.f, 0.f, 0.f) * XMVector3Length(Final.r[1]);
    Final.r[2] = XMVectorSet(0.f, 0.f, 1.f, 0.f) * XMVector3Length(Final.r[2]);


   
    m_pOriginalDesc->Transform(*m_pDesc, Final);



    return S_OK;
}

bool CBounding_AABB::Intersects_Ray(_vector origin, _vector rayDir, _float& Dist)
{
    return m_pDesc->Intersects(origin, rayDir, Dist);

}

bool CBounding_AABB::Intersect(COLLIDER_TYPE eType, CBounding* pOther)
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

_float3 CBounding_AABB::Get_MaxBound(_vector vCenter)
{
    _float3 fCenter;
    XMStoreFloat3(&fCenter, vCenter);


    return _float3(m_pDesc->Center.x +m_pDesc->Extents.x,
        m_pDesc->Center.y + m_pDesc->Extents.y,
        m_pDesc->Center.z + m_pDesc->Extents.z);
}

_float3 CBounding_AABB::Get_MinBound(_vector vCenter)
{
    _float3 fCenter;
    XMStoreFloat3(&fCenter, vCenter);


    return _float3(m_pDesc->Center.x - m_pDesc->Extents.x,
        m_pDesc->Center.y - m_pDesc->Extents.y,
        m_pDesc->Center.z - m_pDesc->Extents.z);
}

#ifdef _DEBUG
HRESULT CBounding_AABB::Render(PrimitiveBatch<VertexPositionColor>* pBatch, _bool isColl)
{
    DX::Draw(pBatch, *m_pDesc, isColl == true ? XMVectorSet(1.f, 0.f, 0.f, 1.f) : XMVectorSet(0.f, 1.f, 0.f, 1.f));

    return S_OK;
}
#endif // DEBUG



CBounding_AABB* CBounding_AABB::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const BOUNDING_DESC* pInitialDesc)
{
    CBounding_AABB* pInstance = new CBounding_AABB(pDevice, pContext);

    if (FAILED(pInstance->Initialize(pInitialDesc)))
    {
        MSG_BOX("Failed to Created : CBounding_AABB");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CBounding_AABB::Free()
{

    __super::Free();

    Safe_Delete(m_pOriginalDesc);
    Safe_Delete(m_pDesc);



}

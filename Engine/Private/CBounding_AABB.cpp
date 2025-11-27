#include "CBounding_AABB.h"
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

HRESULT CBounding_AABB::Update(CTransform* pTransform)
{
      //월드행렬을 따라가되, 내가 설정한 콜라이더자체의 offset과 scale을 포함한다.
    XMMATRIX  NewWorld = XMLoadFloat4x4(&pTransform->Get_World());
    NewWorld = XMMatrixMultiply(NewWorld, XMMatrixScaling(m_vSize.x, m_vSize.y, m_vSize.z));
    NewWorld = XMMatrixMultiply(NewWorld, XMMatrixTranslation(m_vOffSet.x, m_vOffSet.y, m_vOffSet.z));


    m_pOriginalDesc->Transform((*m_pDesc), NewWorld);



    return S_OK;
}

bool CBounding_AABB::Intersects_Ray(_vector origin, _vector rayDir, _float& Dist)
{
    return m_pDesc->Intersects(origin, rayDir, Dist);

}

HRESULT CBounding_AABB::Render(PrimitiveBatch<VertexPositionColor>* pBatch)
{
    DX::Draw(pBatch, *m_pDesc);

    return E_NOTIMPL;
}

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

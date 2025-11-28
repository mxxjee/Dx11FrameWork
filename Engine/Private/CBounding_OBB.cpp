#include "CBounding_OBB.h"
#include "DebugDraw.h"
#include "CTransform.h"


CBounding_OBB::CBounding_OBB(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CBounding(pDevice,pContext)
{
}

HRESULT CBounding_OBB::Initialize(const BOUNDING_DESC* pInitialDesc)
{
    const BOUNDING_OBB_DESC* pDesc = static_cast<const BOUNDING_OBB_DESC*> (pInitialDesc);


    _float4 Rotation;

    XMStoreFloat4(&Rotation,
        XMQuaternionRotationRollPitchYaw(XMConvertToRadians(pDesc->vEularAngles.x),
            XMConvertToRadians(pDesc->vEularAngles.y),
            XMConvertToRadians(pDesc->vEularAngles.z)));



    m_pOriginalDesc = new BoundingOrientedBox(pDesc->vCenter, pDesc->Extents,Rotation);
    m_pDesc = new BoundingOrientedBox(*m_pOriginalDesc);

    return S_OK;
}

HRESULT CBounding_OBB::Update(XMMATRIX WorldMatrix)
{

    //월드행렬을 따라가되, 내가 설정한 콜라이더자체의 offset과 scale을 포함한다.
    XMMATRIX NewWorld = XMMatrixScaling(m_vSize.x, m_vSize.y, m_vSize.z) * XMMatrixTranslation(m_vOffSet.x, m_vOffSet.y, m_vOffSet.z);

    XMMATRIX Final = NewWorld * WorldMatrix;

    m_pOriginalDesc->Transform(*m_pDesc, Final);



    return S_OK;
}

bool CBounding_OBB::Intersects_Ray(_vector origin, _vector rayDir, _float& Dist)
{
    return m_pDesc->Intersects(origin,rayDir,Dist);
}

HRESULT CBounding_OBB::Render(PrimitiveBatch<VertexPositionColor>* pBatch, _bool isColl)
{

    DX::Draw(pBatch, *m_pDesc, isColl == true ? XMVectorSet(1.f, 0.f, 0.f, 1.f) : XMVectorSet(0.f, 1.f, 0.f, 1.f));

    return S_OK;
}

CBounding_OBB* CBounding_OBB::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const BOUNDING_DESC* pInitialDesc)
{
    CBounding_OBB* pInstance = new CBounding_OBB(pDevice, pContext);

    if (FAILED(pInstance->Initialize(pInitialDesc)))
    {
        MSG_BOX("Failed to Created : CBounding_OBB");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CBounding_OBB::Free()
{

    __super::Free();

    Safe_Delete(m_pOriginalDesc);
    Safe_Delete(m_pDesc);
}

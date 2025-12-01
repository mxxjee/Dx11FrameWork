#include "CBounding_Mesh.h"
#include "CTransform.h"
#include "CModel.h"


CBounding_Mesh::CBounding_Mesh(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CBounding(pDevice,pContext)
{
}

HRESULT CBounding_Mesh::Initialize(const BOUNDING_DESC* pInitialDesc)
{
    const BOUNDING_MESH_DESC* pDesc = static_cast<const BOUNDING_MESH_DESC*> (pInitialDesc);

    m_pModel = pDesc->pModel;


    return S_OK;
}

HRESULT CBounding_Mesh::Update(XMMATRIX WorldMatrix)
{
    return S_OK;
}

bool CBounding_Mesh::Intersects_Ray(_vector origin, _vector rayDir, _float& Dist)
{
    return m_pModel->Intersects_Ray(origin,rayDir,Dist);
}

#ifdef _DEBUG
HRESULT CBounding_Mesh::Render(PrimitiveBatch<VertexPositionColor>* pBatch, _bool isColl)
{
    /*Render X*/
    return S_OK;
}
#endif

CBounding_Mesh* CBounding_Mesh::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const BOUNDING_DESC* pInitialDesc)
{
    CBounding_Mesh* pInstance = new CBounding_Mesh(pDevice, pContext);
    if (FAILED(pInstance->Initialize(pInitialDesc)))
    {
        MSG_BOX("Failed to Create : CBounding_Mesh");
        Safe_Release(pInstance);
    }


    return pInstance;
}

void CBounding_Mesh::Free()
{
    __super::Free();
 
}

bool CBounding_Mesh::Intersect(COLLIDER_TYPE eType, CBounding* pOther)
{
    return false;
}

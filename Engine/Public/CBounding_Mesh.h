#pragma once
#include "CBounding.h"

NS_BEGIN(Engine)
class CModel;

class ENGINE_DLL CBounding_Mesh :
    public CBounding
{
public:
    typedef struct tagBoundingMesh : CBounding::BOUNDING_DESC
    {
        _float3 Extents = { 1.f,1.f,1.f };      //초기 범위(Bounding box용)
        CModel* pModel; //모델의 local메쉬에 접근하기위해


    }BOUNDING_MESH_DESC;

private:
    CBounding_Mesh(ComPtr<ID3D11Device>  pDevice, ComPtr<ID3D11DeviceContext>    pContext);
    virtual ~CBounding_Mesh() = default;

public:
    HRESULT     Initialize(const BOUNDING_DESC * pInitialDesc);
    virtual         HRESULT         Update(XMMATRIX WorldMatrix);
    bool            Intersects_Ray(_vector origin, _vector rayDir, _float & Dist);

private:
    BoundingSphere* m_pOriginalDesc = { nullptr };       //로컬 상태의 바운딩sphere
    BoundingSphere* m_pDesc = { nullptr };             // 월드행렬을 가공한 상태의 바운딩sphere

#ifdef _DEBUG
public:
    virtual HRESULT         Render(PrimitiveBatch<VertexPositionColor>*pBatch, _bool isColl) override;
#endif

public:
    static  CBounding_Mesh* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext>        pContext, const BOUNDING_DESC * pInitialDesc);
    virtual void Free() override;

private:
    CModel*             m_pModel;

    // CBounding을(를) 통해 상속됨
    virtual bool Intersect(COLLIDER_TYPE eType, CBounding* pOther) override;
};
NS_END


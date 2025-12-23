#pragma once
#include "CBounding.h"

NS_BEGIN(Engine)
class ENGINE_DLL CBounding_AABB :
    public CBounding
{
public:
    typedef struct tagBoundingAABBDesc: CBounding::BOUNDING_DESC
    {
        _float3 Extents = { 0.6f,0.6f,0.6f };      //초기 범위

    }BOUNDING_AABB_DESC;

private:
    CBounding_AABB(ComPtr<ID3D11Device>  pDevice, ComPtr<ID3D11DeviceContext>    pContext);
    virtual ~CBounding_AABB() = default;

public:
    HRESULT     Initialize(const BOUNDING_DESC* pInitialDesc);
    virtual         HRESULT         Update(XMMATRIX WorldMatrix);
    bool            Intersects_Ray(_vector origin, _vector rayDir, _float& Dist);
    virtual bool            Intersect(COLLIDER_TYPE eType, CBounding* pOther);


public:
    bool        Compute_PushOut_AABBAABB(CBounding* pOther, _float3& vOut);
    bool        Compute_PushOut_AABBSphere(CBounding* pOther, _float3& vOut);

public:
    BoundingBox* Get_Desc() { return m_pDesc; }
    BoundingBox* Get_OrignialDesc() { return m_pOriginalDesc; }
public:
    virtual _float3         Get_MaxBound(_vector vCenter);
    virtual _float3         Get_MinBound(_vector vCenter);
private:
    BoundingBox* m_pOriginalDesc = {nullptr};       //로컬 상태의 AABB바운딩박스
    BoundingBox* m_pDesc = { nullptr };     // 월드행렬을 가공한 상태의 AABB바운딩박스

#ifdef _DEBUG
public:
    virtual HRESULT         Render(PrimitiveBatch<VertexPositionColor>* pBatch, _bool isColl) override;
    virtual void        OnInspectorUI();
#endif // DEBUG



public:
    static  CBounding_AABB* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext>        pContext, const BOUNDING_DESC* pInitialDesc);
    virtual void Free() override;

public:
    void        Set_OffSet(_float3 vOffSet) { m_vOffSet = vOffSet; }
    void        Set_Size(_float3 vSize) { m_vSize = vSize; }

private:
    _float3         m_vOffSet ={ 0.f, 0.f, 0.f };
    _float3         m_vSize = { 1.f,1.f,1.f };  //월드행렬의 scale*m_vSize
};

NS_END
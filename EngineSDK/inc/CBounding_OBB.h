#pragma once
#include "CBounding.h"


NS_BEGIN(Engine)
class ENGINE_DLL CBounding_OBB :
    public CBounding
{
public:
    typedef struct tagBoundingOBBDesc : CBounding::BOUNDING_DESC
    {
        _float3 Extents = { 1.f,1.f,1.f };      //초기 범위
        _float3 vEularAngles = { 0.f,0.f,0.f };

    }BOUNDING_OBB_DESC;

private:
    CBounding_OBB(ComPtr<ID3D11Device>  pDevice, ComPtr<ID3D11DeviceContext>    pContext);
    virtual ~CBounding_OBB() = default;

public:
    HRESULT     Initialize(const BOUNDING_DESC * pInitialDesc);
    virtual         HRESULT         Update(XMMATRIX WorldMatrix);
    bool            Intersects_Ray(_vector origin, _vector rayDir, _float & Dist);
    BoundingOrientedBox* Get_Desc() { return m_pDesc; }
private:
    BoundingOrientedBox* m_pOriginalDesc = { nullptr };       //로컬 상태의 AABB바운딩박스
    BoundingOrientedBox* m_pDesc = { nullptr };     // 월드행렬을 가공한 상태의 AABB바운딩박스


public:
    _float3         Get_MaxBound(_vector vCenter);
    _float3         Get_MinBound(_vector vCenter);
#ifdef _DEBUG
public:
    virtual HRESULT         Render(PrimitiveBatch<VertexPositionColor>*pBatch, _bool isColl) override;
#endif // DEBUG


public:
    static  CBounding_OBB* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext>        pContext, const BOUNDING_DESC * pInitialDesc);
    virtual void Free() override;

public:
    void        Set_OffSet(_float3 vOffSet) { m_vOffSet = vOffSet; }
    void        Set_Size(_float3 vSize) { m_vSize = vSize; }

private:
    _float3         m_vOffSet = { 0.f, 0.f, 0.f };
    _float3         m_vSize = { 1.f,1.f,1.f };  //월드행렬의 scale*m_vSize



                                                // CBounding을(를) 통해 상속됨
    virtual bool Intersect(COLLIDER_TYPE eType, CBounding* pOther) override;

};

NS_END
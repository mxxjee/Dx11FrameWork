#pragma once
#include "CBounding.h"

NS_BEGIN(Engine)
class CTransform;
class ENGINE_DLL CBounding_Sphere :
    public CBounding
{
public:
    typedef struct tagBondingSphere : CBounding::BOUNDING_DESC
    {
        _float fRadius = 1.f;   //초기 범위

    }BOUNDING_SPHERE_DESC;

private:
    CBounding_Sphere(ComPtr<ID3D11Device>  pDevice, ComPtr<ID3D11DeviceContext>    pContext);
    virtual ~CBounding_Sphere() = default;

public:
    HRESULT     Initialize(const BOUNDING_DESC * pInitialDesc);
    virtual         HRESULT         Update(XMMATRIX WorldMatrix);
    bool            Intersects_Ray(_vector origin, _vector rayDir, _float & Dist);

private:
    BoundingSphere* m_pOriginalDesc = { nullptr };       //로컬 상태의 바운딩sphere
    BoundingSphere* m_pDesc = { nullptr };
    // 월드행렬을 가공한 상태의 바운딩sphere


#ifdef _DEBUG
public:
    virtual HRESULT         Render(PrimitiveBatch<VertexPositionColor>*pBatch, _bool isColl) override;

#endif // DEBUG
public:
    static  CBounding_Sphere* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext>        pContext, const BOUNDING_DESC * pInitialDesc);
    virtual void Free() override;

public:
    void        Set_Raidus(_float fRadius) { m_fRadius = fRadius; }
 

private:
    _float      m_fRadius = 1.f;


};
NS_END

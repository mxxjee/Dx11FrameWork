#pragma once
#include "CBase.h"

NS_BEGIN(Engine)
class CTransform;
class ENGINE_DLL CBounding abstract:
    public CBase
{
public:
    typedef struct tagBoundingDesc
    {
        _float3         vCenter = { 0.f,0.f,0.f };        //Bounding 센터(로컬)

    }BOUNDING_DESC;

protected:
    CBounding(ComPtr<ID3D11Device>  pDevice, ComPtr<ID3D11DeviceContext>    pContext);
    virtual ~CBounding() = default;

public:
    virtual         HRESULT         Update(XMMATRIX WorldMatrix)=0;
    virtual bool            Intersects_Ray(_vector origin, _vector rayDir, _float& Dist)=0;
    virtual bool            Intersect(COLLIDER_TYPE eType, CBounding* pOther)=0;


    virtual _float3         Get_MaxBound(_vector vCenter) = 0;
    virtual _float3         Get_MinBound(_vector vCenter) = 0;

#ifdef _DEBUG
public:
    virtual HRESULT         Render(PrimitiveBatch<VertexPositionColor>* pBatch,_bool isColl) = 0;
    
#endif
public:
    HRESULT     Initialize();



protected:
    ComPtr<ID3D11Device>    m_pDevice = nullptr;
    ComPtr<ID3D11DeviceContext> m_pContext = nullptr;
public:
    virtual void Free() override;


};
NS_END


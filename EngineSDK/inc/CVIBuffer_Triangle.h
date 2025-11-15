#pragma once
#include "CVIBuffer.h"

NS_BEGIN(Engine)
class ENGINE_DLL CVIBuffer_Triangle :
    public CVIBuffer
{
public:
    typedef struct tagTriangleBuffer : CComponent::tagComponentDesc
    {
        _float3 v0, v1, v2;

    }TRIANGLEBUFFER_DESC;


private:
    CVIBuffer_Triangle(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CVIBuffer_Triangle(const CVIBuffer_Triangle& Prototype);
    virtual ~CVIBuffer_Triangle() = default;

public:
    void            UpdatePoints(_float3 p0, _float3 p1, _float3 p2);


public:
    virtual         HRESULT     Initialize_Prototype();
    virtual         HRESULT     Initialize_Copytype(void* pArg) override;


public:
    static CVIBuffer_Triangle* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    virtual CComponent* Clone(void* pArg);
    virtual void Free() override;
};
NS_END


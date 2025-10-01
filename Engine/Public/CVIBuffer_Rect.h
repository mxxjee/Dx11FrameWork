#pragma once
#include "CVIBuffer.h"

NS_BEGIN(Engine)
class ENGINE_DLL CVIBuffer_Rect final :
public CVIBuffer
{
private:
    CVIBuffer_Rect(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CVIBuffer_Rect(const CVIBuffer_Rect& Prototype);
    virtual ~CVIBuffer_Rect() = default;


public:
    virtual         HRESULT     Initialize_Prototype() override;
    virtual         HRESULT     Initialize_Copytype(void* pArg) override;

public:
    static CVIBuffer_Rect* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    virtual CComponent* Clone(void* pArg);
    virtual void Free() override;

};
NS_END


#pragma once
#include "CBase.h"
class CComponent :
    public CBase
{
protected:
    CComponent(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CComponent(const CComponent& Prototype);
    virtual ~CComponent() = default;



public:
    virtual         HRESULT     Initialize_Prototype();
    virtual         HRESULT     Initialize(void* pArg);



protected:
    ComPtr<ID3D11Device> m_pDevice = nullptr;
    ComPtr<ID3D11DeviceContext> m_pContext = nullptr;

public:
    virtual CComponent* Clone(void* pArg) = 0;
    virtual void Free() override;


};


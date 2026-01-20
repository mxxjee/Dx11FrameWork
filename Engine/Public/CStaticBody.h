#pragma once


#include "CBody.h"

NS_BEGIN(Engine)


class ENGINE_DLL CStaticBody :
    public CBody
{
protected:
    CStaticBody(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CStaticBody(const CStaticBody& rhs);
    virtual ~CStaticBody() = default;

public:
    static CStaticBody* Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;

};

NS_END


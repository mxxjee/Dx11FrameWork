#pragma once
#include "CInteractionObject.h"

NS_BEGIN(Client)
class CInteraction_Lawn :
    public CInteractionObject
{
protected:
    CInteraction_Lawn(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CInteraction_Lawn(const CInteraction_Lawn& rhs);
    virtual ~CInteraction_Lawn() = default;


public:
    static CInteraction_Lawn* Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext);
    // CGameObject을(를) 통해 상속됨
    virtual CGameObject* Clone(void* pArg) override;
    virtual     void    Free() override;

};

NS_END
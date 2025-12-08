#pragma once
#include "CInteractionObject.h"

NS_BEGIN(Client)
class CInteraction_Rock :
    public CInteractionObject
{
protected:
    CInteraction_Rock(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CInteraction_Rock(const CInteraction_Rock& rhs);
    virtual ~CInteraction_Rock() = default;


public:
    static CInteraction_Rock* Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext);
    // CGameObject을(를) 통해 상속됨
    virtual CGameObject* Clone(void* pArg) override;
    virtual     void    Free() override;

};
NS_END


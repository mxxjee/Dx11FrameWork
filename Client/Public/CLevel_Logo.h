#pragma once

#include "CLevel.h"
#include "Client_Defines.h"

NS_BEGIN(Client)
class CLevel_Logo final :
    public CLevel
{
private:
    explicit CLevel_Logo(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext);
    virtual ~CLevel_Logo() = default;


public:
    virtual HRESULT     Initialize() override;                             //¾À ¼¼ÆÃ.
    virtual HRESULT     Update(const _float fTimeDelta) override;        //¾ÀÀÇ ¾÷µ¥ÀÌÆ®
    virtual void        Render() override;         //¾ÀÀÇ ·»´õ.
    virtual void        Clear() override;        //ÀÚ¿ø Á¤¸® ÇÔ¼ö

public:
    static  CLevel_Logo* Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext);
    virtual     void        Free();

};
NS_END


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
    virtual HRESULT     Initialize(LevelArgs& args) override;                             //æ¿ ºº∆√.
    virtual HRESULT     Update(const _float fTimeDelta) override;        //æ¿¿« æ˜µ•¿Ã∆Æ
    virtual void        Render() override;         //æ¿¿« ∑ª¥ı.

public:
    static  CLevel_Logo* Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext,LevelArgs& args);
    virtual     void        Free();

};
NS_END


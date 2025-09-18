#pragma once
#include "CBase.h"

NS_BEGIN(Engine)

class CLevel :
    public CBase
{

protected:
    explicit CLevel(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext);
    virtual ~CLevel()=default;


public:
    virtual HRESULT     Initialize();                       //¾À ¼¼ÆÃ.
    virtual HRESULT     Update(const _float fTimeDelta);        //¾ÀÀÇ ¾÷µ¥ÀÌÆ®
    virtual void        Render();         //¾ÀÀÇ ·»´õ.
    virtual void        Clear();        //ÀÚ¿ø Á¤¸® ÇÔ¼ö

private:
    virtual     void        Free();

private:
    class CGameInstance*          m_pGameInstance = { nullptr };
    ComPtr<ID3D11Device>    m_pDevice = { nullptr };
    ComPtr<ID3D11DeviceContext>    m_pDeviceContext = { nullptr };




};
NS_END


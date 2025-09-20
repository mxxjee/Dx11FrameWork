#pragma once
#include "CBase.h"

NS_BEGIN(Engine)

class ENGINE_DLL CLevel :
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

public:
    virtual     void        Free();
    
public:
    void                    Set_State(LEVELSTATE eState) { m_eLevelState = eState; }
    const LEVELSTATE&       Get_State() { return m_eLevelState; }
protected:
    class CGameInstance*          m_pGameInstance = { nullptr };
    ComPtr<ID3D11Device>    m_pDevice = { nullptr };
    ComPtr<ID3D11DeviceContext>    m_pDeviceContext = { nullptr };



private:
    LEVELSTATE              m_eLevelState = { LEVELSTATE::ACTIVE };


};
NS_END


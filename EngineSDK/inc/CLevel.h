#pragma once
#include "CBase.h"
#include "Engine_LevelTypes.h"

NS_BEGIN(Engine)


class ENGINE_DLL CLevel :
    public CBase
{

protected:
    explicit CLevel(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext);
    virtual ~CLevel()=default;


public:
    virtual HRESULT     Initialize(LevelArgs& args);                       //¾À ¼¼ÆÃ.
   
    virtual void            Update(const _float fTimeDelta);
 
    
    virtual void        Render();         //¾ÀÀÇ ·»´õ.
   
public:
    virtual void        OnEnter() {};
    virtual void        OnResume() {};
    virtual void        OnPause() {};
    virtual void        OnExit() {};        //ÀÚ¿ø Á¤¸® ÇÔ¼ö

public:
    virtual     void        Free();
    
public:
    void                    Set_State(LEVELSTATE eState) { m_eLevelArgs.m_eState = eState; }
    const LEVELSTATE& Get_State()   const {return m_eLevelArgs.m_eState;}

    void                    Set_Flag(LEVELFLAG eFlag) { m_eLevelArgs.m_eFlag = eFlag; }
    const   LEVELFLAG&      Get_Flag() const { return m_eLevelArgs.m_eFlag; }

protected:
    class CGameInstance*            m_pGameInstance = { nullptr };
    ComPtr<ID3D11Device>            m_pDevice = { nullptr };
    ComPtr<ID3D11DeviceContext>     m_pDeviceContext = { nullptr };



private:
    LevelArgs           m_eLevelArgs;

protected:
    _uint               m_iLevelID = 0;
};
NS_END


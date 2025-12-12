#pragma once
#include "CBase.h"
#include "Engine_LevelTypes.h"

NS_BEGIN(Engine)


class ENGINE_DLL CLevel :
    public CBase
{

protected:
    explicit CLevel(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    virtual ~CLevel()=default;


public:
    virtual HRESULT     Initialize(LevelArgs& args);                       //씬 세팅.
   
    virtual void            Update_Priority(_float fTimeDelta);
    virtual void            Update(const _float fTimeDelta);
    virtual void            Update_Late(_float fTimeDelta);
    virtual void            Update_Render(_float fTimeDelta);
    
    virtual void        Render();         //씬의 렌더.
   
public:
    virtual void        OnEnter() {};           //씬 처음 진입시 호출
    virtual void        OnResume() {};              //pause되었다가 active되었을때 호출
    virtual void        OnPause() {};               //pause되었을때 호출
    virtual void        OnExit() {};        //자원 정리 함수

public:
    virtual     void        Free();
    
public:
    bool                    Is_Cached() { return m_eLevelArgs.m_bCached; }
    void                    Set_State(LEVELSTATE eState) { m_eLevelArgs.m_eState = eState; }
    const LEVELSTATE& Get_State()   const {return m_eLevelArgs.m_eState;}

    void                    Set_Flag(LEVELFLAG eFlag) { m_eLevelArgs.m_eFlag = eFlag; }
    const   LEVELFLAG&      Get_Flag() const { return m_eLevelArgs.m_eFlag; }

    _uint                   Get_LevelID() const { return m_eLevelArgs.m_iLevelID; }
protected:
    class CGameInstance*            m_pGameInstance = { nullptr };
    ComPtr<ID3D11Device>            m_pDevice;
    ComPtr<ID3D11DeviceContext>     m_pContext;



private:
    LevelArgs           m_eLevelArgs;

protected:
    _uint               m_iLevelID = 0;
};
NS_END


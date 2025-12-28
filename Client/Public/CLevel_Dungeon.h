#pragma once
#include "CLevel.h"

namespace Engine
{
    class CGameObject;
}

NS_BEGIN(Client)
class CFadeScreen;
class CGameManager;
class CRoom;
class CLevel_Dungeon :
    public CLevel
{
    enum TELEPORT
    {
       GOTO_2ND,GOTO_1PRE,GOTO_EXIT
    };

    enum PHASE
    {
        FIRST,SECOND,END
    };
private:
    explicit CLevel_Dungeon(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext);
    virtual ~CLevel_Dungeon() = default;

public:
    virtual HRESULT     Initialize(LevelArgs& args) override;                             //씬 세팅.
    virtual void        Update_Priority(_float fTimeDelta);
    virtual void        Update(const _float fTimeDelta) override;
    virtual void            Update_Late(_float fTimeDelta);


    //씬의 업데이트
    virtual void            Render() override;         //씬의 렌더.


public:
    HRESULT                 Ready_Lights();
    HRESULT                 Ready_Layer_Enviroment(const _wstring& strLayerTag);
    HRESULT                 Ready_Layer_UI(const _wstring& strLayerTag);
    HRESULT                 Ready_Layer_Player(const _wstring& strLayerTag);
    HRESULT                 Ready_Layer_Monster(const _wstring& strLayerTag);

    HRESULT                 Ready_Layer_InteractionObject(const _wstring& strLayerTag);

    HRESULT                 Ready_Layer_Particle(const _wstring& strLayerTag);
    HRESULT                 Ready_Layer_Trigger(const _wstring& strLayerTag);

    HRESULT                 Ready_Events();
public:
    void        Teleport(TELEPORT eType);
public:
    virtual void        OnEnter() override;           //씬 진입시 매번호출
    virtual void        OnResume(_uint iPreLevel) override;              //pause되었다가 active되었을때 호출
    virtual void        OnPause(_uint iNextLeve) override;               //pause되었을때 호출
    virtual void        OnExit() override;


public:
    static  CLevel_Dungeon* Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext, LevelArgs& args);
    virtual     void        Free();

private:    
    CFadeScreen* pFadeScreen = nullptr;
    CGameManager* m_pGameManager = nullptr;

    PHASE m_eCurrentPhase;
    class CGameObject* m_Rooms[4] = { nullptr };


private:
    GameEvent       m_EnterSecondEvent;
    GameEvent       m_EnterFirstEvent;
};

NS_END

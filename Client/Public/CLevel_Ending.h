#pragma once
#include "CLevel.h"



NS_BEGIN(Client)
class CGameManager;
class CFadeScreen;
#include "CLevel.h"
class CLevel_Ending :
    public CLevel
{
private:
    explicit CLevel_Ending(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext);
    virtual ~CLevel_Ending() = default;

public:
    virtual HRESULT     Initialize(LevelArgs & args) override;                             //씬 세팅.
    virtual void        Update_Priority(_float fTimeDelta);
    virtual void        Update(const _float fTimeDelta) override;
    virtual void            Update_Late(_float fTimeDelta);


    //씬의 업데이트
    virtual void            Render() override;         //씬의 렌더.


public:
    HRESULT                 Ready_Layer_Enviroment(const _wstring& strLayerTag);
    HRESULT                 Ready_Lights();
    HRESULT                 Ready_Layer_NPC(const _wstring& strLayerTag); HRESULT                 Ready_Layer_Player(const _wstring& strLayerTag);

public:
    virtual void        OnEnter() override;           //씬 진입시 매번호출
    virtual void        OnResume(_uint iPreLevel) override;              //pause되었다가 active되었을때 호출
    virtual void        OnPause(_uint iNextLeve) override;               //pause되었을때 호출
    virtual void        OnExit() override;


public:
    static  CLevel_Ending* Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext, LevelArgs& args);
    virtual     void        Free();


private:
    CGameManager* m_pGameManager = nullptr;
    CFadeScreen* pFadeScreen = nullptr;
};
NS_END


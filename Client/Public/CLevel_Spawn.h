#pragma once
#include "CLevel.h"
#include "Client_Defines.h"

/*게임 시작 후 첫 스폰씬(마린집)*/
NS_BEGIN(Client)
class CFadeScreen;
class CGameManager;
class CLevel_Spawn :
    public CLevel
{
    
private:
    explicit CLevel_Spawn(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext);
    virtual ~CLevel_Spawn() = default;


public:
    virtual HRESULT     Initialize(LevelArgs & args) override;                       //씬 세팅.

    virtual void            Update_Priority(_float fTimeDelta);
    virtual void            Update(const _float fTimeDelta) override;        //씬의 업데이트
    virtual void            Update_Late(_float fTimeDelta);


    virtual void        Render() override;         //씬의 렌더.

public:
    HRESULT                 Ready_Lights();
    HRESULT                 Ready_Layer_Enviroment(const _wstring & strLayerTag);
    HRESULT                 Ready_Layer_NPC(const _wstring & strLayerTag);
    HRESULT                 Ready_Layer_InteractionObject(const _wstring & strLayerTag);
    HRESULT                 Ready_Layer_Trigger(const _wstring & strLayerTag);
    HRESULT                 Ready_Layer_Particle(const _wstring& strLayerTag);


    HRESULT                 Ready_Player_Static(const _wstring& strLayerTag);
    HRESULT                 Ready_UI_Static(const _wstring& strLayerTag);

public:
                //NPC의 talk거리 조절
    void        EndCutScene();
public:
    virtual void        OnEnter() override;           //씬 진입시 매번호출
    virtual void        OnResume(_uint iPreLevel) override;              //pause되었다가 active되었을때 호출
    virtual void        OnPause(_uint iNextLeve) override;               //pause되었을때 호출
    virtual void        OnExit() override;
public:
    static  CLevel_Spawn* Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext, LevelArgs & args);
    virtual     void        Free();
private:
    int iTargetIdx = 0;
    CFadeScreen* pFadeScreen = nullptr;
    CGameManager* m_pGameManager = nullptr;

};

NS_END

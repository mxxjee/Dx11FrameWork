#pragma once
#include "CLevel.h"
namespace Engine
{
    class CGameObject;
    class CLight;
}

NS_BEGIN(Client)
class CFadeScreen;
class CGameManager;
class CInteraction_JackyBall;
class CWall;

class CLevel_Boss :
    public CLevel
{
private:
    typedef struct tagMovingLight
    {
        class CLight* pLight = nullptr;
        _vector vDir;
    }MOVING_LIGHT;
private:
    explicit CLevel_Boss(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext);
    virtual ~CLevel_Boss() = default;

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
    HRESULT                 Ready_Layer_Monster(const _wstring& strLayerTag);
    HRESULT                 Ready_Layer_Interaction(const _wstring& strLayerTag);
    HRESULT                 Ready_Layer_Trigger(const _wstring& strLayerTag);
    HRESULT                 Ready_Layer_NPC(const _wstring& strLayerTag);

    HRESULT                 Ready_Events();
public:
    virtual void        OnEnter() override;           //씬 진입시 매번호출
    virtual void        OnResume(_uint iPreLevel) override;              //pause되었다가 active되었을때 호출
    virtual void        OnPause(_uint iNextLeve) override;               //pause되었을때 호출
    virtual void        OnExit() override;

public:
    virtual void                    Play_LevelBGM();
public:
    void        Close_Door();
public:
    static  CLevel_Boss* Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext, LevelArgs& args);
    virtual     void        Free();


private:
    CFadeScreen* pFadeScreen = nullptr;
    CGameManager* m_pGameManager = nullptr;

    GameEvent       m_EnterFirstEvent;
    CInteraction_JackyBall* pJackyBall;

    vector<CWall*>       m_pWalls;

    _float4         m_vTargetDiffuse = _float4(0.45f, 0.45f, 0.45f, 1.f);

private:
    void        Change_Value(_float fTimeDelta);
    void        Move_Lights(_float fTimeDelta);
private:
    CLight* m_pDirectionalLight = nullptr;
    vector<MOVING_LIGHT>     m_PointLights;

private:
    bool        m_bPreValue = true;
    bool        m_bValue = false;

    _float          m_fTime = 0.f;
    _float          m_fCoolTime = 3.f;

    _float4         MinBound=_float4(0.f,0.f,0.f,1.f);
    _float4         MaxBound = _float4(0.f, 0.f, 0.f, 1.f);


};


NS_END

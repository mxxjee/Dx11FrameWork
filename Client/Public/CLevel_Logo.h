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
    virtual HRESULT     Initialize(LevelArgs& args) override;                             //씬 세팅.
    virtual void        Update_Priority(_float fTimeDelta);
    virtual void        Update(const _float fTimeDelta) override;
    virtual void            Update_Late(_float fTimeDelta);

    
    //씬의 업데이트
    virtual void        Render() override;         //씬의 렌더.

public:
    HRESULT                 Ready_Lights();
    HRESULT                 Ready_Layer_Enviroment(const _wstring& strLayerTag);
    HRESULT                 Ready_Layer_UI(const _wstring& strLayerTag);
    HRESULT                 Ready_Layer_MainCamera(const _wstring& strLayerTag);
    
    HRESULT                 Ready_Layer_Player(const _wstring& strLayerTag);
    HRESULT                 Ready_Layer_Monster(const _wstring& strLayerTag);

public:
    virtual void        OnEnter() override;           //씬 진입시 매번호출
    virtual void        OnResume() override;              //pause되었다가 active되었을때 호출
    virtual void        OnPause() override;               //pause되었을때 호출
    virtual void        OnExit() override;
public:
    static  CLevel_Logo* Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext,LevelArgs& args);
    virtual     void        Free();
    void        Set_UIPos_ByWorld(_float3 OffSet);
private:
    void        Create_MainCamera();
    void        Create_UICamera();
    void        Create_FreeCamera();
    void        Create_MiniMapCamera();
private:
    int iTargetIdx = 0;

};
NS_END


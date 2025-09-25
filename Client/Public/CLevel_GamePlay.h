#pragma once

#include "CLevel.h"
#include "Client_Defines.h"

NS_BEGIN(Client)
class CLevel_GamePlay final :
    public CLevel
{
private:
    explicit CLevel_GamePlay(ID3D11Device* _pDevice, ID3D11DeviceContext* _pDeviceContext);
    virtual ~CLevel_GamePlay() = default;


public:
    virtual HRESULT     Initialize(LevelArgs& args) override;                       //씬 세팅.
    
    virtual void            Update_Priority(_float fTimeDelta);
    virtual void            Update(const _float fTimeDelta) override;        //씬의 업데이트
    virtual void            Update_Late(_float fTimeDelta);

    
    virtual void        Render() override;         //씬의 렌더.

public:
    HRESULT                 Ready_Layer_Background(const _wstring& strLayerTag);

public:
    virtual void        OnEnter() override;           //씬 진입시 매번호출
    virtual void        OnResume() override;              //pause되었다가 active되었을때 호출
    virtual void        OnPause() override;               //pause되었을때 호출
    virtual void        OnExit() override;
public:
    static  CLevel_GamePlay* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pDeviceContext, LevelArgs& args);
    virtual     void        Free();


};
NS_END


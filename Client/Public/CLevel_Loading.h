#pragma once

#include "CLevel.h"
#include "Client_Defines.h"

NS_BEGIN(Client)
class CLevel_Loading final :
    public CLevel
{
private:
    explicit CLevel_Loading(ID3D11Device* _pDevice, ID3D11DeviceContext* _pDeviceContext);
    virtual ~CLevel_Loading() = default;


public:
    HRESULT     Initialize(LEVEL_ID iLevelID,LEVELCHANGETYPE eChangeType,LevelArgs& args);                       //씬 세팅.

    virtual void            Update_Priority(_float fTimeDelta);
    virtual void            Update(const _float fTimeDelta) override;        //씬의 업데이트
    virtual void            Update_Late(_float fTimeDelta);

    
    
    virtual void        Render() override;         //씬의 렌더.

private:
    HRESULT     Ready_UI_Layer();

public:
    virtual void        OnEnter() override;           //씬 진입시 매번호출
    virtual void        OnResume() override;              //pause되었다가 active되었을때 호출
    virtual void        OnPause() override;               //pause되었을때 호출
    virtual void        OnExit() override;
public:
    static  CLevel_Loading* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pDeviceContext,LevelArgs& args);
    virtual     void        Free();


private:
    class CLoader* m_pLoader = { nullptr };
    LEVEL_ID			m_eNextLevelID = { LEVEL_ID::END };
    LEVELCHANGETYPE     m_eChangeType = { LEVELCHANGETYPE::END };
};
NS_END


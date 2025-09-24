#pragma once

#include "CLevel.h"
#include "Client_Defines.h"

NS_BEGIN(Client)
class CLevel_Loading final :
    public CLevel
{
private:
    explicit CLevel_Loading(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext);
    virtual ~CLevel_Loading() = default;


public:
    HRESULT     Initialize(LEVEL_ID iLevelID,LEVELCHANGETYPE eChangeType,LevelArgs& args);                       //æ¿ ºº∆√.

    virtual void        Update(const _float fTimeDelta) override;        //æ¿¿« æ˜µ•¿Ã∆Æ
    virtual void        Render() override;         //æ¿¿« ∑ª¥ı.

private:
    HRESULT     Ready_UI_Layer();

public:
    static  CLevel_Loading* Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext,LevelArgs& args);
    virtual     void        Free();


private:
    class CLoader* m_pLoader = { nullptr };
    LEVEL_ID			m_eNextLevelID = { LEVEL_ID::END };
    LEVELCHANGETYPE     m_eChangeType = { LEVELCHANGETYPE::END };
};
NS_END


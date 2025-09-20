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
    HRESULT     Initialize(LEVEL_ID iLevelID,LEVELCHANGETYPE eChangeType);                       //¾À ¼¼ÆÃ.
    virtual HRESULT     Update(const _float fTimeDelta) override;        //¾ÀÀÇ ¾÷µ¥ÀÌÆ®
    virtual void        Render() override;         //¾ÀÀÇ ·»´õ.
    virtual void        Clear() override;        //ÀÚ¿ø Á¤¸® ÇÔ¼ö

private:
    HRESULT     Ready_UI_Layer();

public:
    static  CLevel_Loading* Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext,LEVEL_ID iLevelID,LEVELCHANGETYPE eChangeType);
    virtual     void        Free();


private:
    class CLoader* m_pLoader = { nullptr };
    LEVEL_ID			m_eNextLevelID = { LEVEL_ID::END };
    LEVELCHANGETYPE     m_eChangeType = { LEVELCHANGETYPE::END };
};
NS_END


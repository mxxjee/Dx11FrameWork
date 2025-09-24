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
    virtual HRESULT     Initialize(LevelArgs& args) override;                       //æ¿ ºº∆√.
    virtual void     Update(const _float fTimeDelta) override;        //æ¿¿« æ˜µ•¿Ã∆Æ
    virtual void        Render() override;         //æ¿¿« ∑ª¥ı.

public:
    HRESULT                 Ready_Layer_Background(const _wstring& strLayerTag);


public:
    static  CLevel_GamePlay* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pDeviceContext, LevelArgs& args);
    virtual     void        Free();


};
NS_END


#pragma once
#include "CLevel.h"
#include "Client_Defines.h"

NS_BEGIN(Client)
class CFadeScreen;
class CPlayer;
class CLevel_NPCRoom :
    public CLevel
{
private:
    explicit CLevel_NPCRoom(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext);
    virtual ~CLevel_NPCRoom() = default;


public:
    virtual HRESULT     Initialize(LevelArgs & args) override;                       //씬 세팅.

    virtual void            Update_Priority(_float fTimeDelta);
    virtual void            Update(const _float fTimeDelta) override;        //씬의 업데이트
    virtual void            Update_Late(_float fTimeDelta);


    virtual void        Render() override;         //씬의 렌더.


public:
    virtual void                    Play_LevelBGM();
    virtual void        OnEnter() override;           //씬 진입시 매번호출
    virtual void        OnResume(_uint iPreLevel) override;              //pause되었다가 active되었을때 호출
    virtual void        OnPause(_uint iNextLevel) override;               //pause되었을때 호출
    virtual void        OnExit() override;
public:
    static  CLevel_NPCRoom* Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext, LevelArgs & args);
    virtual     void        Free();

private:
    CFadeScreen* pFadeScreen = nullptr;

    CPlayer* m_pPlayer = nullptr;


};
NS_END

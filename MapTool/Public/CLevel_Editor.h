#pragma once
#include "CLevel.h"

namespace Engine
{
    class CMapObject_Manager;
}

NS_BEGIN(MapTool)
class CLevel_Editor :
    public CLevel
{
private:
    explicit CLevel_Editor(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext);
    virtual ~CLevel_Editor() = default;


public:
    virtual HRESULT     Initialize(LevelArgs & args) override;                             //씬 세팅.
    virtual void        Update_Priority(_float fTimeDelta);
    virtual void        Update(const _float fTimeDelta) override;
    virtual void            Update_Late(_float fTimeDelta);


    //씬의 업데이트
    virtual void        Render() override;         //씬의 렌더.

public:
    HRESULT                 Create_TerrainHighlight(Triangle* PickingPos);
public:
    HRESULT                 Ready_Layer_Enviroment(const _wstring & strLayerTag);
    HRESULT                 Ready_Layer_UI(const _wstring & strLayerTag);
    HRESULT                 Ready_Layer_MainCamera(const _wstring & strLayerTag);
    HRESULT                 Ready_Layer_Player(const _wstring & strLayerTag);


public:
    virtual void        OnEnter() override;           //씬 진입시 매번호출
    virtual void        OnResume() override;              //pause되었다가 active되었을때 호출
    virtual void        OnPause() override;               //pause되었을때 호출
    virtual void        OnExit() override;
public:
    static  CLevel_Editor* Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext, LevelArgs & args);
    virtual     void        Free();

    int             m_iIdx = 0;

private:
    CMapObject_Manager* m_pMapObject_Manager = { nullptr };

};

NS_END


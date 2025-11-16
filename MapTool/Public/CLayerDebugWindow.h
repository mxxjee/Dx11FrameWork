#pragma once
#include "CImgui_Window.h"

namespace Engine
{
    class CGameInstance;
    class CMapObject;
    class CMapObject_Manager;
    class CTerrain_Manager;
    class IMapEditable;

}

NS_BEGIN(MapTool)
class CLayerDebugWindow :
    public CImgui_Window
{
protected:
    CLayerDebugWindow(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    virtual ~CLayerDebugWindow() = default;

public:
    // CImgui_Base을(를) 통해 상속됨
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Update() override;
    virtual void Render() override;



public:
    static CLayerDebugWindow* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg);
    virtual void Free();

private:
    CGameInstance* pGameInstance = { nullptr };
    IMapEditable* pSelectObject = { nullptr };

private:
    CMapObject_Manager* m_pMapObject_Manager = { nullptr };

};

NS_END

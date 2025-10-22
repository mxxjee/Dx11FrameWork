#pragma once

namespace Engine
{
    class CGameInstance;
    class CMapObject;
}

#include "CImgui_Window.h"

NS_BEGIN(MapTool)
class CObjectInspectorWindow :

    public CImgui_Window
{
protected:
    CObjectInspectorWindow(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    virtual ~CObjectInspectorWindow() = default;

public:
    // CImgui_Base을(를) 통해 상속됨
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Update() override;
    virtual void Render() override;

public:
    static CObjectInspectorWindow* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg);
    virtual void Free();

private:
    CGameInstance* pGameInstance = { nullptr };
    CMapObject* pSelectObject = { nullptr };
};
NS_END

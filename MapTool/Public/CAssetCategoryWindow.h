#pragma once
#include "CImgui_Window.h"

namespace Engine
{
    class CGameInstance;
    class CMapObject;
    class CMapObject_Manager;
}


NS_BEGIN(MapTool)
class CImgui_DataManager;

class CAssetCategoryWindow :
    public CImgui_Window
{
protected:
    CAssetCategoryWindow(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    virtual ~CAssetCategoryWindow() = default;

private:
    HRESULT     Create_Widgets();


public:
    // CImgui_Base을(를) 통해 상속됨
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Update() override;
    virtual void Render() override;
public:
    static CAssetCategoryWindow* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg);
    virtual void Free();

private:
    CGameInstance* pGameInstance = { nullptr };
    CMapObject_Manager* m_pMapObject_Manager = { nullptr };
    CImgui_DataManager* m_pImgui_DataManager = { nullptr };


};


NS_END
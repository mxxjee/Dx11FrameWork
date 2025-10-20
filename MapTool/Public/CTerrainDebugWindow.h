#pragma once
#include "CImgui_Window.h"
namespace Engine
{
    class CImgui_Button;
    class CImgui_Checkbox;
    class CImgui_InputText;
    class CGameInstance;
    class CCamera_Base;
}


NS_BEGIN(MapTool)
class CTerrainDebugWindow :
    public CImgui_Window
{
private:
    CTerrainDebugWindow(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    virtual ~CTerrainDebugWindow() = default;


public:
    // CImgui_Base을(를) 통해 상속됨
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Update() override;
    virtual void Render() override;

public:
    void        Set_MapTerrain(class CGameObject* pObj);
private:
    HRESULT     Create_Widgets();

public:
    static CTerrainDebugWindow* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg);
    virtual void Free();

private:
    CGameInstance* m_pGameInstance = { nullptr };

private:
    vector<class CImgui_InputText*>     m_InputTexts;
    class CMapTerrain*                  m_pMapTerrain = { nullptr };
};
NS_END


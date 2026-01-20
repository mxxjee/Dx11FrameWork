#pragma once
#include "CImgui_Window.h"
namespace Engine
{
    class CImgui_Button;
    class CImgui_Checkbox;
    class CImgui_InputInt;
    class CGameInstance;
    class CCamera_Base;
    class CGameObject;
}


NS_BEGIN(MapTool)

class CGridDebugWindow :
    public CImgui_Window
{
private:
    CGridDebugWindow(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    virtual ~CGridDebugWindow() = default;


public:
    // CImgui_Base을(를) 통해 상속됨
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Update() override;
    virtual void Render() override;

public:
    void        Set_MapTerrain(class Engine::CGameObject* pObj);
private:
    HRESULT     Create_Widgets();

public:
    void        Init_NumValues();
public:
    static CGridDebugWindow* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg);
    virtual void Free();

private:
    CGameInstance* m_pGameInstance = { nullptr };

private:
    vector<class CImgui_InputInt*>     m_InputTexts;
    class CImgui_Button*                m_pButton;


    class CMapGrid*                  m_MapGrid= { nullptr };

    int pNumVerticesX=0;
    int pNumVerticesZ=0;
};
NS_END


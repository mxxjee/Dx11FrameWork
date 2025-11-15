#pragma once
#include "CBase.h"
#include "CImgui_Base.h"

NS_BEGIN(Engine)
class ENGINE_DLL CImGui_Manager final :
public CBase
{
        DECLARE_SINGLETON(CImGui_Manager)

    private:
        explicit CImGui_Manager();
        virtual ~CImGui_Manager();

    public:
        void            Init(HWND hWnd,ID3D11Device* device, ID3D11DeviceContext* device_context);
        void            Update();
        void            Render(ID3D11DeviceContext* device_context);

        void            Test();
        void            BeginDockSpace();
public:
        void            Free() override;


    public:
        void        RegisterWindow(CImgui_Base* pInstance);
        CImgui_Base* Find_Window(string _tag);
    private:
        bool        show_another_window ={ false };
        bool        show_demo_window = { true };
        _float4     clearColor = { 0.f,0.f,0.f,0.f };

    private:
        unordered_map<string, CImgui_Base*>        m_Windows;

public:
    void        Set_MapToolMode(MapToolMode Mode) { eMode = Mode; }
    MapToolMode     Get_MapToolMode() { return eMode; }
private:
    MapToolMode         eMode = MapToolMode::EDIT;
};

NS_END


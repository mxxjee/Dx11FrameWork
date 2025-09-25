#pragma once
#include "CBase.h"

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


    private:
        bool        show_another_window ={ false };
        bool        show_demo_window = { true };
        _float4     clearColor = { 0.f,0.f,0.f,0.f };
 

};

NS_END


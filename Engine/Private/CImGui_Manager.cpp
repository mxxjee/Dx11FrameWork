#include "CImGui_Manager.h"
#include "ImGuizmo.h"
#include "CGameInstance.h"


IMPLEMENT_SINGLETON(CImGui_Manager)

CImGui_Manager::CImGui_Manager()
{

}

CImGui_Manager::~CImGui_Manager()
{
}

void CImGui_Manager::Init(HWND hWnd, ID3D11Device* device, ID3D11DeviceContext* device_context)
{
    //모니터 세팅에 맞는 Imgui 세팅
    ImGui_ImplWin32_EnableDpiAwareness();
    float main_scale = ImGui_ImplWin32_GetDpiScaleForMonitor(::MonitorFromPoint(POINT{ 0, 0 }, MONITOR_DEFAULTTOPRIMARY));


    //Imgui context세팅
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows

    // Setup Dear ImGui style
    ImGui::StyleColorsMint();
    //ImGui::StyleColorsLight();
   // ImGui::StyleColorsSpectrum();
    // Setup scaling
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(main_scale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
    style.FontScaleDpi = main_scale;        // Set initial font scale. (using io.ConfigDpiScaleFonts=true makes this unnecessary. We leave both here for documentation purpose)
    io.ConfigDpiScaleFonts = true;          // [Experimental] Automatically overwrite style.FontScaleDpi in Begin() when Monitor DPI changes. This will scale fonts but _NOT_ scale sizes/padding for now.
    io.ConfigDpiScaleViewports = true;      // [Experimental] Scale Dear ImGui and Platform Windows when Monitor DPI changes.

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }
    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hWnd);
    ImGui_ImplDX11_Init(device, device_context);

}

void CImGui_Manager::Update_Priority()
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();



    BeginDockSpace();//도킹기능

    for (auto& pair : m_Windows)
        pair.second->Update_Priority();
}

void CImGui_Manager::Update()
{
  
    //나중에 외부에서 설정할듯, 일단 테스트.
   // Test();

    for (auto& pair : m_Windows)
        pair.second->Update();
}

void CImGui_Manager::Render(ID3D11DeviceContext* device_context)
{
    ImGui::Render();
  //  const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
   // g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
   // g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    // 뷰포트 옵션 켠 경우 추가
    ImGuiIO& io = ImGui::GetIO();

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        // DX11 상태 백업
        ID3D11RenderTargetView* backup_rt = nullptr;
        ID3D11DepthStencilView* backup_ds = nullptr;
        device_context->OMGetRenderTargets(1, &backup_rt, &backup_ds);

        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();

        //// DX11 상태 복원
        device_context->OMSetRenderTargets(1, &backup_rt, backup_ds);
        if (backup_rt) backup_rt->Release();
        if (backup_ds) backup_ds->Release();
    }


    for (auto& pair : m_Windows)
        pair.second->Render();
}

void CImGui_Manager::Test()
{
    // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
    if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);
    ImGuiIO& io = ImGui::GetIO();

    ImGui::Text("DockingEnable: %s", (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) ? "ON" : "OFF");
    ImGui::Text("ViewportsEnable: %s", (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) ? "ON" : "OFF");
    
    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
    {
        static float f = 0.0f;
        static int counter = 0;

        ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

        ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
        ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
        ImGui::Checkbox("Another Window", &show_another_window);

        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::ColorEdit3("clear color", (float*)&clearColor); // Edit 3 floats representing a color

        if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
            counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

   //     ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        ImGui::End();
    }

    // 3. Show another simple window.
    if (show_another_window)
    {
        ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        ImGui::Text("Hello from another window!");
        if (ImGui::Button("Close Me"))
            show_another_window = false;
        ImGui::End();
    }

}

void CImGui_Manager::BeginDockSpace()
{
    const ImGuiViewport* viewport = ImGui::GetMainViewport();

    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0, 0, 0, 0));

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking |
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoNavFocus |
        ImGuiWindowFlags_NoBackground;

    ImGui::Begin("DockSpace Demo", nullptr, window_flags);

    ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);

    ImGui::End();

    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor(2);
}

void CImGui_Manager::Free()
{
    for (auto& pair : m_Windows)
    {
        Safe_Release(pair.second);
    }
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void CImGui_Manager::RegisterWindow(CImgui_Base* pInstance)
{
    CheckNull(pInstance);
    if (m_Windows.find(pInstance->Get_Tag()) == m_Windows.end())
        m_Windows[pInstance->Get_Tag()] = pInstance;
}

CImgui_Base* CImGui_Manager::Find_Window(string _tag)
{
    auto pTargetIter = m_Windows.find(_tag);
    CImgui_Base* pTarget = nullptr;

    if (pTargetIter != m_Windows.end())
        pTarget = pTargetIter->second;

    return pTarget;
}

void CImGui_Manager::Reset_Window(string _WindowName)
{
    CImgui_Base* pWindow = Find_Window(_WindowName);
    if (pWindow)
        pWindow->Reset();
}

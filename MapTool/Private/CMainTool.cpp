#include "CMainTool.h"
#include "CImGui_Manager.h"
#include "CGameInstance.h"
#include "Client_Defines.h"
#include "CImgui_Window.h"
#include "CImgui_Button.h"

USING(MapTool)

CMainTool::CMainTool()
    :pImGui_Manager(CImGui_Manager::GetInstance()),
    pGameInstance(CGameInstance::GetInstance())
{
}

HRESULT CMainTool::Initialize()
   
{
    ENGINE_DESC		desc;
    desc.hWnd = g_hWnd;
    desc.iWinSizeX = g_iWinSizeX;
    desc.iWinSizeY = g_iWinSizeY;
    desc.winMode = WINMODE::WIN;
    desc.iNumLevels = ENUM_TO_UINT(LEVEL_ID::END);

    if(FAILED(pGameInstance->Initialize_Engine(desc, &m_pDevice, &m_pContext)))
        return E_FAIL;

    pImGui_Manager->Init(g_hWnd, m_pDevice.Get(), m_pContext.Get());


    CreateMyWindow();
 

    return S_OK;
}

void CMainTool::Update(_float fTimeDelta)
{
    pImGui_Manager->Update();
}

void CMainTool::Render()
{
    pGameInstance->Draw_Begin(&ClearColor);
    pImGui_Manager->Render(m_pContext.Get());
    pGameInstance->Draw_End();
}

void CMainTool::CreateMyWindow()
{
    CImgui_Window::IMGUIWINDOW_DESC Desc;
    Desc.m_WindowTitle = "MyImguiWindow";
    Desc.m_WindowPos = ImVec2(100, 100);
    Desc.m_WindowSize = ImVec2(300, 300);
    Desc.Tag = L"MyImguiWindow";


    //Create MyButton
    {
        CImgui_Button::IMGUIBUTTON_DESC ButtonDesc;
        ButtonDesc.callback = []()
        {

        };

        ButtonDesc.Label = "MyButton";
        ButtonDesc.m_RelativePos = ImVec2(10, 80);
        ButtonDesc.Tag = L"MyButton";


        CImgui_Button* pButton = CImgui_Button::Create(m_pDevice, m_pContext, &ButtonDesc);
        Desc.m_Widgets.push_back(pButton);
    }
   
    pImGui_Manager->RegisterWindow(CImgui_Window::Create(m_pDevice, m_pContext, &Desc));
}

CMainTool* CMainTool::Create()
{
    CMainTool* pMainTool = new CMainTool;

    if (FAILED(pMainTool->Initialize()))
    {
        /*생성하지 못했다면 Safe_Release()로 삭제*/
        MSG_BOX("Failed to Create : CMainTool");
        Safe_Release(pMainTool);
    }

    return pMainTool;
}

void CMainTool::Free()
{

    Safe_Release(pImGui_Manager);
    Safe_Release(pGameInstance);
}

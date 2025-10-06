#include "CMainTool.h"
#include "CImGui_Manager.h"
#include "CGameInstance.h"
#include "Client_Defines.h"
#include "CImgui_Window.h"
#include "CImgui_Button.h"

#include "../Public/CLevel_Loading.h"
#include "CLevel_Editor.h"

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
    desc.hInst = g_hInst;
    desc.iWinSizeX = g_iWinSizeX;
    desc.iWinSizeY = g_iWinSizeY;
    desc.winMode = WINMODE::WIN;
    desc.iNumLevels = ENUM_TO_UINT(Client::LEVEL_ID::END);

    if(FAILED(pGameInstance->Initialize_Engine(desc, &m_pDevice, &m_pContext)))
        return E_FAIL;

    pImGui_Manager->Init(g_hWnd, m_pDevice.Get(), m_pContext.Get());


    CreateMyWindow();

    Reigster_Levels();
    if (FAILED(Start_Level(Client::LEVEL_ID::MAPTOOL, LEVELCHANGETYPE::REPLACETOP)))
        return E_FAIL;


    return S_OK;
}

void CMainTool::Update_Priority(_float fTimeDelta)
{
    pGameInstance->Update_Priority_Engine(fTimeDelta);
}

void CMainTool::Update(_float fTimeDelta)
{
    pGameInstance->Update_Engine(fTimeDelta);
    pImGui_Manager->Update();
}

void CMainTool::Update_Late(float fTimeDelta)
{
    pGameInstance->LateUpdate_Engine(fTimeDelta);
}

void CMainTool::Update_Render(float fTimeDelta)
{
    pGameInstance->Update_Render(fTimeDelta);
}

void CMainTool::Render()
{
    pGameInstance->Draw_Begin(&ClearColor);
    pGameInstance->Draw();
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

void CMainTool::Reigster_Levels()
{
    CheckNull(pGameInstance);
    pGameInstance->Register_Level(ENUM_TO_UINT(LEVEL_ID::MAPTOOL), [this](LevelArgs& args)->CLevel*
        {
            return CLevel_Editor::Create(m_pDevice, m_pContext, args);
        });

    pGameInstance->Register_Level(ENUM_TO_UINT(LEVEL_ID::LOADING), [this](LevelArgs& args)->CLevel*
        {
            return CLevel_Loading::Create(m_pDevice, m_pContext, args);
        });
}

HRESULT CMainTool::Start_Level(Client::LEVEL_ID iLevelID, LEVELCHANGETYPE eChangeType)
{
    /*일단 로딩씬으로 이동하고, 로딩씬에게 아이디를 넘겨줘서 어떤걸 로딩할지 로더에게 요청.
    그리고 로딩씬이 다음씬으로 이동하도록 한다.*/

    LevelArgs args;
    args.iNextLevelID = ENUM_TO_UINT(iLevelID);
    args.changeType = LEVELCHANGETYPE::OVERLAY;
    args.m_eFlag = LEVELFLAG::TRANSIENT;
    args.loadingChangeType = LEVELCHANGETYPE::PUSH;
    args.m_iLevelID = ENUM_TO_UINT(LEVEL_ID::LOADING);


    if (FAILED(pGameInstance->Level_Changer(ENUM_TO_UINT(LEVEL_ID::LOADING), args)))
        return E_FAIL;


    return S_OK;
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

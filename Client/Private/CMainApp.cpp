#include "CMainApp.h"
#include "CGameInstance.h"


#include "CLevel_Logo.h"
#include "CLevel_GamePlay.h"
#include "CLevel_Loading.h"

#include "CImGui_Manager.h"
#include "CImgui_Button.h"
#include "CLevelDebugWindow.h"
#include "CObjectDebugWindow.h"


USING(Client)

CMainApp::CMainApp()
	:pImGui_Manager(CImGui_Manager::GetInstance()),
	pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(pGameInstance);
}

HRESULT CMainApp::Initialize()
{
	/*내 게임의 초기화 작업을 수행*/


	ENGINE_DESC		desc;
	desc.hWnd = g_hWnd;
	desc.hInst = g_hInst;
	desc.iWinSizeX = g_iWinSizeX;
	desc.iWinSizeY = g_iWinSizeY;
	desc.winMode = WINMODE::WIN;
	desc.iNumLevels = ENUM_TO_UINT(LEVEL_ID::END);


	if(FAILED(pGameInstance->Initialize_Engine(desc,&m_pDevice,&m_pContext)))
		return E_FAIL;

	Reigster_Levels();
#ifdef _DEBUG
	pImGui_Manager->Init(g_hWnd, m_pDevice.Get(), m_pContext.Get());
	CreateLevelDebugWindow();
	CreateObjectDebugWindow();
#endif

	if (FAILED(Start_Level(LEVEL_ID::LOGO,LEVELCHANGETYPE::REPLACETOP)))
		return E_FAIL;

	
	return S_OK;
}

void CMainApp::Update_Priority(_float fTimeDelta)
{
	pGameInstance->Update_Priority_Engine(fTimeDelta);
}

void CMainApp::Update(_float fTimeDelta)
{
	/*내 게임의 반복적인 작업 수행*/
	pGameInstance->Update_Engine(fTimeDelta);

#ifdef _DEBUG
	pImGui_Manager->Update();
#endif

}

void CMainApp::Update_Late(float fTimeDelta)
{
	pGameInstance->LateUpdate_Engine(fTimeDelta);
}

void CMainApp::Update_Render(float fTimeDelta)
{
	pGameInstance->Update_Render(fTimeDelta);
}


void CMainApp::Render()
{
	/*내 게임의 반복적인 렌더.*/
	pGameInstance->Draw_Begin(&ClearColor);
	pGameInstance->Draw();

#ifdef _DEBUG
	pImGui_Manager->Render(m_pContext.Get());
#endif

	pGameInstance->Draw_End();

}

void CMainApp::Reigster_Levels()
{
	CheckNull(pGameInstance);
	pGameInstance->Register_Level(ENUM_TO_UINT(LEVEL_ID::LOGO), [this](LevelArgs& args)->CLevel*
		{
			return CLevel_Logo::Create(m_pDevice, m_pContext,args);
		});

	pGameInstance->Register_Level(ENUM_TO_UINT(LEVEL_ID::GAMEPLAY), [this](LevelArgs& args)->CLevel*
		{
			return CLevel_GamePlay::Create(m_pDevice, m_pContext, args);
		});


	pGameInstance->Register_Level(ENUM_TO_UINT(LEVEL_ID::LOADING), [this](LevelArgs& args)->CLevel*
		{
			return CLevel_Loading::Create(m_pDevice, m_pContext, args);
		});



}

HRESULT CMainApp::Start_Level(LEVEL_ID iLevelID, LEVELCHANGETYPE eChangeType)
{
	/*일단 로딩씬으로 이동하고, 로딩씬에게 아이디를 넘겨줘서 어떤걸 로딩할지 로더에게 요청.
	그리고 로딩씬이 다음씬으로 이동하도록 한다.*/

	LevelArgs args;
	args.iNextLevelID = ENUM_TO_UINT(iLevelID);
	args.changeType = LEVELCHANGETYPE::OVERLAY;
	args.m_eFlag = LEVELFLAG::TRANSIENT;
	args.loadingChangeType = LEVELCHANGETYPE::PUSH;
	args.m_iLevelID = ENUM_TO_UINT(LEVEL_ID::LOADING);


	if(FAILED(pGameInstance->Level_Changer(ENUM_TO_UINT(LEVEL_ID::LOADING),args)))
		return E_FAIL;


	return S_OK;
}

CMainApp* CMainApp::Create()
{
	//메인앱을 완벽하게 생성하기 위한 함수
	//new 뿐만아니라 초기화작업(Initialize)수행
	CMainApp* pMainApp = new CMainApp;

	if (FAILED(pMainApp->Initialize()))
	{
		/*생성하지 못했다면 Safe_Release()로 삭제*/
		MSG_BOX("Failed to Create : CMainApp");
		Safe_Release(pMainApp);
	}

	return pMainApp;
}

void CMainApp::Free()
{

	//상속계층을 따르기 위해 부모  Free호출 
	__super::Free();

#ifdef _DEBUG
	Safe_Release(pImGui_Manager);
#endif

	pGameInstance->Release_Engine();




	//자신의 리소스정리
	
	Safe_Release(pGameInstance);

}

void CMainApp::CreateLevelDebugWindow()
{
	CImgui_Window::IMGUIWINDOW_DESC Desc;
	Desc.m_WindowTitle = "LevelDebugWindow";
	Desc.m_WindowPos = ImVec2(100, 100);
	//Desc.m_WindowSize = ImVec2(300, 500);
	Desc.Tag = L"LevelDebugWindow";


	pImGui_Manager->RegisterWindow(CLevelDebugWindow::Create(m_pDevice, m_pContext, &Desc));
}

void CMainApp::CreateObjectDebugWindow()
{
	CImgui_Window::IMGUIWINDOW_DESC Desc;
	Desc.m_WindowTitle = "ObjectDebugWindow";
	Desc.m_WindowPos = ImVec2(g_iWinSizeX, 100);
	//Desc.m_WindowSize = ImVec2(300, 500);
	Desc.Tag = L"ObjectDebugWindow";


	pImGui_Manager->RegisterWindow(CObjectDebugWindow::Create(m_pDevice, m_pContext, &Desc));

}





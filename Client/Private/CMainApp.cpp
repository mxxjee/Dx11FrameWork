#include "CMainApp.h"
#include "CGameInstance.h"
#include "CShader.h"
#include "VertexData.h"

#include "CGameObject.h"
#include "CUI.h"
#include "CCamera_Base.h"

#include "CLevel_Logo.h"
#include "CLevel_Town.h"
#include "CLevel_Loading.h"
#include "CLevel_UI.h"


#include "CImGui_Manager.h"
#include "CImgui_Button.h"
#include "CInput_Manager.h"

#include "CLevelDebugWindow.h"
#include "CObjectDebugWindow.h"
#include "CCameraDebugWindow.h"

#include "CTransform.h"
#include "CCameraComponent.h"


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

	if (FAILED(Initialize_Cilent()))
		return E_FAIL;
	CInput_Manager::GetInstance()->Init_Input(g_hInst, g_hWnd);

	Register_Levels();


	//Imgui 디버그창
#ifdef _DEBUG
	pImGui_Manager->Init(g_hWnd, m_pDevice.Get(), m_pContext.Get());
	CreateLevelDebugWindow();
	CreateObjectDebugWindow();
	CreateCameraDebugWindow();

#endif

	if (FAILED(Start_Level(LEVEL_ID::LOGO,LEVELCHANGETYPE::REPLACETOP)))
		return E_FAIL;

	//NavTest
	if (FAILED(Ready_Test()))
		return E_FAIL;

	
	return S_OK;
}

HRESULT CMainApp::Initialize_Cilent()
{
	CreateSamplerStates();
	CreateBlendStates();
	CreateRasterizerStates();
	CreateDepthStencilStates();


	//렌더그룹 관련 초기화
	if(FAILED(pGameInstance->Initialize_Renderer(ENUM_TO_UINT(RENDERGROUP::END))))
		return E_FAIL;

	//각 렌더그룹에맞는 세팅값등록
	for (int i = 0; i < ENUM_TO_UINT(RENDERGROUP::END); ++i)
	{
		if (FAILED(pGameInstance->Register_RenderStates(i, m_RenderStates[i])))
			return E_FAIL;

	}
	

	//렌더타입에 맞는 정렬함수 등록
	auto AlphaSort = [&](CGameObject* a, CGameObject* b)
	{

		CCamera_Base* pRenderCam = CGameInstance::GetInstance()->Get_RenderCamera();
		
		_float4x4 fViewMat = CGameInstance::GetInstance()->Get_ViewMatrix(ENUM_TO_UINT(pRenderCam->Get_CameraType()));
		_matrix ViewMat = XMLoadFloat4x4(&fViewMat);


		CTransform* aTrans = dynamic_cast<CTransform*>(a->Get_Component(COMPONENT_TYPE::TRANSFORM));
		CTransform* bTrans = dynamic_cast<CTransform*>(b->Get_Component(COMPONENT_TYPE::TRANSFORM));

		_vector aView = XMVector3TransformCoord(aTrans->Get_State(STATE::POSITION, TransformScope::WORLD), ViewMat);
		_vector bView = XMVector3TransformCoord(bTrans->Get_State(STATE::POSITION, TransformScope::WORLD), ViewMat);

		//내림차순정렬, 먼것부터 그려야함
		return XMVectorGetZ(aView) > XMVectorGetZ(bView);
	};

	auto UISort = [&](CGameObject* a, CGameObject* b)
		{
			CUI* pUI_A = dynamic_cast<CUI*>(a);
			CUI* pUI_B = dynamic_cast<CUI*>(b);

			if (pUI_A && pUI_B)
			{
				return pUI_A->Get_Depth() > pUI_B->Get_Depth();
			}

			return false;
		};

	if(FAILED(pGameInstance->Add_SortFunc(ENUM_TO_UINT(RENDERGROUP::ALPHA), AlphaSort)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_SortFunc(ENUM_TO_UINT(RENDERGROUP::WORLD_UI_MINIMAP), AlphaSort)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_SortFunc(ENUM_TO_UINT(RENDERGROUP::UI), UISort)))
		return E_FAIL;

	return S_OK;

}

void CMainApp::Update_Priority(_float fTimeDelta)
{
	CInput_Manager::GetInstance()->Update_Input();
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

void CMainApp::Register_Levels()
{
	CheckNull(pGameInstance);
	pGameInstance->Register_Level(ENUM_TO_UINT(LEVEL_ID::LOGO), [this](LevelArgs& args)->CLevel*
		{
			return CLevel_Logo::Create(m_pDevice, m_pContext,args);
		});

	pGameInstance->Register_Level(ENUM_TO_UINT(LEVEL_ID::TOWN), [this](LevelArgs& args)->CLevel*
		{
			return CLevel_Town::Create(m_pDevice, m_pContext, args);
		});

	pGameInstance->Register_Level(ENUM_TO_UINT(LEVEL_ID::UI), [this](LevelArgs& args)->CLevel*
		{
			return CLevel_UI::Create(m_pDevice, m_pContext, args);
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


	CInput_Manager::GetInstance()->DestroyInstance();
	pGameInstance->Release_Engine();




	//자신의 리소스정리

	Safe_Release(pGameInstance);

}

void CMainApp::CreateSamplerStates()
{
	m_RenderStates.resize(ENUM_TO_UINT(RENDERGROUP::END));

	D3D11_SAMPLER_DESC desc;
	memset(&desc, 0, sizeof(desc));
	desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.BorderColor[0] = 0.0f;
	desc.BorderColor[1] = 0.0f;
	desc.BorderColor[2] = 0.0f;
	desc.BorderColor[3] = 0.0f;


	desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	desc.MaxAnisotropy = 16;
	desc.MaxLOD = FLT_MAX;
	desc.MinLOD = FLT_MIN;
	desc.MipLODBias = 0.0f;

	for (int i = 0; i < ENUM_TO_UINT(RENDERGROUP::WORLD_UI_MINIMAP); ++i)
		m_pDevice->CreateSamplerState(&desc, m_RenderStates[i]._samplerState.GetAddressOf());

	for (int i = ENUM_TO_UINT(RENDERGROUP::WORLD_UI_MINIMAP); i < ENUM_TO_UINT(RENDERGROUP::END); ++i)
		m_pDevice->CreateSamplerState(&desc, m_RenderStates[i]._samplerState.GetAddressOf());

}

void CMainApp::CreateBlendStates()
{
	D3D11_BLEND_DESC desc;
	memset(&desc, 0, sizeof(desc));
	desc.AlphaToCoverageEnable = false;
	desc.IndependentBlendEnable = false;

	desc.RenderTarget[0].BlendEnable = FALSE;
	desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	m_pDevice->CreateBlendState(&desc, m_RenderStates[ENUM_TO_UINT(RENDERGROUP::PRIORITY)]._BlendState.GetAddressOf());
	m_pDevice->CreateBlendState(&desc, m_RenderStates[ENUM_TO_UINT(RENDERGROUP::NONALPHA)]._BlendState.GetAddressOf());

	desc.RenderTarget[0].BlendEnable = TRUE;

	for (int i = ENUM_TO_UINT(RENDERGROUP::ALPHA); i < ENUM_TO_UINT(RENDERGROUP::END); ++i)
		m_pDevice->CreateBlendState(&desc, m_RenderStates[i]._BlendState.GetAddressOf());


}

void CMainApp::CreateRasterizerStates()
{
	D3D11_RASTERIZER_DESC desc;
	memset(&desc, 0, sizeof(desc));

	desc.FillMode = D3D11_FILL_SOLID;//WIREFRAME of SOLID
	desc.CullMode = D3D11_CULL_NONE;//CULLMODE: 반시계 컬링
	desc.FrontCounterClockwise = false;
	desc.DepthClipEnable = true;

	for (int i = 0; i < ENUM_TO_UINT(RENDERGROUP::UI); ++i)
		m_pDevice->CreateRasterizerState(&desc, m_RenderStates[i]._rasterizerState.GetAddressOf());

	desc.DepthClipEnable = false;
	m_pDevice->CreateRasterizerState(&desc, m_RenderStates[ENUM_TO_UINT(RENDERGROUP::UI)]._rasterizerState.GetAddressOf());
}

void CMainApp::CreateDepthStencilStates()
{

	D3D11_DEPTH_STENCIL_DESC desc = CD3D11_DEPTH_STENCIL_DESC(D3D11_DEFAULT);

	m_pDevice->CreateDepthStencilState(&desc, m_RenderStates[ENUM_TO_UINT(RENDERGROUP::PRIORITY)]._DepthStencilState.GetAddressOf());
	m_pDevice->CreateDepthStencilState(&desc, m_RenderStates[ENUM_TO_UINT(RENDERGROUP::NONALPHA)]._DepthStencilState.GetAddressOf());

	//깊이 테스트여부
	desc.DepthEnable = true;

	//깊이 기록여부
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	m_pDevice->CreateDepthStencilState(&desc, m_RenderStates[ENUM_TO_UINT(RENDERGROUP::ALPHA)]._DepthStencilState.GetAddressOf());
	m_pDevice->CreateDepthStencilState(&desc, m_RenderStates[ENUM_TO_UINT(RENDERGROUP::WORLD_UI_MINIMAP)]._DepthStencilState.GetAddressOf());

	desc.DepthEnable = false;
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	m_pDevice->CreateDepthStencilState(&desc, m_RenderStates[ENUM_TO_UINT(RENDERGROUP::UI)]._DepthStencilState.GetAddressOf());

}

HRESULT CMainApp::Ready_Test()
{
	_ulong			dwByte = {};
	HANDLE			hFile = CreateFile(TEXT("../../Resource/Data/Map/Navigation.dat"), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;


	_float3 vPoints[3] = {};
	
	vPoints[0] = _float3(0.f, 0.f, 10.f);
	vPoints[1] = _float3(10.f, 0.f, 0.f);
	vPoints[2] = _float3(0.0f, 0.f, 0.f);
	WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);

	vPoints[0] = _float3(0.f, 0.f, 10.f);
	vPoints[1] = _float3(10.f, 0.f, 10.f);
	vPoints[2] = _float3(10.0f, 0.f, 0.f);
	WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);

	vPoints[0] = _float3(0.f, 0.f, 20.f);
	vPoints[1] = _float3(10.f, 0.f, 10.f);
	vPoints[2] = _float3(0.0f, 0.f, 10.f);
	WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);

	vPoints[0] = _float3(10.f, 0.f, 10.f);
	vPoints[1] = _float3(20.f, 0.f, 0.f);
	vPoints[2] = _float3(10.0f, 0.f, 0.f);
	WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);
	CloseHandle(hFile);

	return S_OK;
}

void CMainApp::CreateLevelDebugWindow()
{
	CImgui_Window::IMGUIWINDOW_DESC Desc;
	Desc.m_WindowTitle = "LevelDebugWindow";
	Desc.m_WindowPos = ImVec2(100, 100);
	//Desc.m_WindowSize = ImVec2(300, 500);
	Desc.Tag = "LevelDebugWindow";


	pImGui_Manager->RegisterWindow(CLevelDebugWindow::Create(m_pDevice, m_pContext, &Desc));
}

void CMainApp::CreateObjectDebugWindow()
{
	CImgui_Window::IMGUIWINDOW_DESC Desc;
	Desc.m_WindowTitle = "ObjectDebugWindow";
	Desc.m_WindowPos = ImVec2(g_iWinSizeX, 100);
	//Desc.m_WindowSize = ImVec2(300, 500);
	Desc.Tag = "ObjectDebugWindow";


	pImGui_Manager->RegisterWindow(CObjectDebugWindow::Create(m_pDevice, m_pContext, &Desc));

}

void CMainApp::CreateCameraDebugWindow()
{
	CImgui_Window::IMGUIWINDOW_DESC Desc;
	Desc.m_WindowTitle = "CameraDebugWindow";
	Desc.m_WindowPos = ImVec2(g_iWinSizeX, 100);
	//Desc.m_WindowSize = ImVec2(300, 500);
	Desc.Tag = "CameraDebugWindow";

	pImGui_Manager->RegisterWindow(CCameraDebugWindow::Create(m_pDevice, m_pContext, &Desc));

}





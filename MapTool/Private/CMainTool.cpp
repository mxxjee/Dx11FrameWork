#include "CMainTool.h"
#include "CImGui_Manager.h"
#include "CGameInstance.h"
#include "Client_Defines.h"

#include "CImgui_Window.h"
#include "CImgui_Button.h"

#include "CGridDebugWindow.h"
#include "CLayerDebugWindow.h"
#include "CObjectInspectorWindow.h"
#include "CAssetCategoryWindow.h"
#include "CAssetListWindow.h"
#include "CSaveWindow.h"
#include "CMenuBarWindow.h"

#include "CGameObject.h"

#include "CCamera_Base.h"

#include "../Public/CLevel_Loading.h"
#include "CLevel_Editor.h"

#include "CInput_Manager.h"

#include "CMapObject_Manager.h"
#include "CImgui_DataManager.h"
#include "CGrid_Manager.h"



USING(MapTool)

CMainTool::CMainTool()
    :pImGui_Manager(CImGui_Manager::GetInstance()),
    pGameInstance(CGameInstance::GetInstance()),
    pGrid_Manager(CGrid_Manager::GetInstance())
{
    Safe_AddRef(pGameInstance);
    Safe_AddRef(pGrid_Manager);
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

    if (FAILED(Initialize_MapTool()))
        return E_FAIL;

    pImGui_Manager->Init(g_hWnd, m_pDevice.Get(), m_pContext.Get());


    CreateGridDebugWindow();
    CreateLayerDebugWindow();
    CreateObjectInspectorWindow();
    CreateAssetBrowserWindow();
    //CreateSaveWindow();
    CreateMenuBar();

    Reigster_Levels();
    if (FAILED(Start_Level(Client::LEVEL_ID::MAPTOOL, LEVELCHANGETYPE::REPLACETOP)))
        return E_FAIL;

    CInput_Manager::GetInstance()->Init_Input(g_hInst, g_hWnd);



    return S_OK;
}

HRESULT CMainTool::Initialize_MapTool()
{
    CreateSamplerStates();
    CreateBlendStates();
    CreateRasterizerStates();
    CreateDepthStencilStates();
  

    //MapObjectMAnager만들기
    pMapObject_Manager = CMapObject_Manager::GetInstance();

    if (FAILED(pGrid_Manager->Initialize(m_pDevice,m_pContext)))
        return E_FAIL;

    if(FAILED(pMapObject_Manager->Initialize(m_pDevice,m_pContext)))
        return E_FAIL;

    if (FAILED(CImgui_DataManager::GetInstance()->Initialize(m_pDevice, m_pContext)))
        return E_FAIL;

    //렌더그룹 관련 초기화
    if (FAILED(pGameInstance->Initialize_Renderer(ENUM_TO_UINT(RENDERGROUP::END))))
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


    if (FAILED(pGameInstance->Add_SortFunc(ENUM_TO_UINT(RENDERGROUP::ALPHA), AlphaSort)))
        return E_FAIL;

    if (FAILED(pGameInstance->Add_SortFunc(ENUM_TO_UINT(RENDERGROUP::WORLD_UI_MINIMAP), AlphaSort)))
        return E_FAIL;

    return S_OK;
}

void CMainTool::Update_Priority(_float fTimeDelta)
{
    CInput_Manager::GetInstance()->Update_Input();
    pGrid_Manager->Update_Priority(fTimeDelta);
    pMapObject_Manager->Update_Priority(fTimeDelta);
    pGameInstance->Update_Priority_Engine(fTimeDelta);


}

void CMainTool::Update(_float fTimeDelta)
{
    pGrid_Manager->Update(fTimeDelta);
    pMapObject_Manager->Update(fTimeDelta);
    pGameInstance->Update_Engine(fTimeDelta);
    pImGui_Manager->Update();
    CImgui_DataManager::GetInstance()->Update_MouseInput();
}

void CMainTool::Update_Late(float fTimeDelta)
{
    pGrid_Manager->Update_Late(fTimeDelta);
    pMapObject_Manager->Update_Late(fTimeDelta);
    pGameInstance->LateUpdate_Engine(fTimeDelta);
}

void CMainTool::Update_Render(float fTimeDelta)
{
    pGrid_Manager->Update_Render(fTimeDelta);
    pMapObject_Manager->Update_Render(fTimeDelta);
    pGameInstance->Update_Render(fTimeDelta);
}

void CMainTool::Render()
{
    pGameInstance->Draw_Begin(&ClearColor);
    pGameInstance->Draw();
    pImGui_Manager->Render(m_pContext.Get());
    pGameInstance->Draw_End();
}

void CMainTool::CreateSamplerStates()
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

    desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

    for (int i = ENUM_TO_UINT(RENDERGROUP::WORLD_UI_MINIMAP); i < ENUM_TO_UINT(RENDERGROUP::END); ++i)
        m_pDevice->CreateSamplerState(&desc, m_RenderStates[i]._samplerState.GetAddressOf());
}

void CMainTool::CreateBlendStates()
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

void CMainTool::CreateRasterizerStates()
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

void CMainTool::CreateDepthStencilStates()
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

void CMainTool::CreateGridDebugWindow()
{
    CImgui_Window::IMGUIWINDOW_DESC Desc;
    Desc.m_WindowTitle = "GridDebugWindow";
    Desc.m_WindowPos = ImVec2(100, 100);
    Desc.m_WindowSize = ImVec2(300, 300);
    Desc.Tag = "GridDebugWindow";

    pImGui_Manager->RegisterWindow(CGridDebugWindow::Create(m_pDevice, m_pContext, &Desc));
}

void CMainTool::CreateLayerDebugWindow()
{

    CImgui_Window::IMGUIWINDOW_DESC Desc;
    Desc.m_WindowTitle = "LayerDebugWindow";
    Desc.m_WindowPos = ImVec2(100, 100);
    Desc.m_WindowSize = ImVec2(300, 300);
    Desc.Tag = "LayerDebugWindow";

    pImGui_Manager->RegisterWindow(CLayerDebugWindow::Create(m_pDevice, m_pContext, &Desc));

}

void CMainTool::CreateObjectInspectorWindow()
{
    CObjectInspectorWindow::IMGUIWINDOW_DESC Desc;
    Desc.m_WindowTitle = "Inspector";
    Desc.m_WindowPos = ImVec2(100, 100);
    Desc.m_WindowSize = ImVec2(300, 300);
    Desc.Tag = "Inspector";

    pImGui_Manager->RegisterWindow(CObjectInspectorWindow::Create(m_pDevice, m_pContext, &Desc));

}

void CMainTool::CreateAssetBrowserWindow()
{
    CAssetCategoryWindow::IMGUIWINDOW_DESC Desc;
    Desc.m_WindowTitle = "AssetCategory";
    Desc.m_WindowPos = ImVec2(100, 100);
    Desc.m_WindowSize = ImVec2(300, 300);
    Desc.Tag = "AssetCategory";

    pImGui_Manager->RegisterWindow(CAssetCategoryWindow::Create(m_pDevice, m_pContext, &Desc));

    ///////////////////////////////////
    CAssetListWindow::IMGUIWINDOW_DESC ListDesc;
    ListDesc.m_WindowTitle = "AssetList";
    ListDesc.m_WindowPos = ImVec2(100, 100);
    ListDesc.m_WindowSize = ImVec2(300, 300);
    ListDesc.Tag = "AssetList";

    pImGui_Manager->RegisterWindow(CAssetListWindow::Create(m_pDevice, m_pContext, &ListDesc));
}

void CMainTool::CreateSaveWindow()
{
    CSaveWindow::IMGUIWINDOW_DESC Desc;
    Desc.m_WindowTitle = "SaveWindow";
    Desc.m_WindowPos = ImVec2(100, 100);
    Desc.m_WindowSize = ImVec2(300, 300);
    Desc.Tag = "SaveWindow";

    pImGui_Manager->RegisterWindow(CSaveWindow::Create(m_pDevice, m_pContext, &Desc));


}

void CMainTool::CreateMenuBar()
{
    CMenuBarWindow::IMGUIWINDOW_DESC Desc;
    Desc.m_WindowTitle = "MenuBarWindow";
    Desc.m_WindowPos = ImVec2(100, 100);
    Desc.m_WindowSize = ImVec2(300, 300);
    Desc.Tag = "MenuBarWindow";

    pImGui_Manager->RegisterWindow(CMenuBarWindow::Create(m_pDevice, m_pContext, &Desc));


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
    __super::Free();

    Safe_Release(pImGui_Manager);
    CInput_Manager::GetInstance()->DestroyInstance();
    pGameInstance->Release_Engine();

    Safe_Release(pMapObject_Manager);
    Safe_Release(pGameInstance);
    Safe_Release(pGrid_Manager);


    CImgui_DataManager::GetInstance()->DestroyInstance();
    CMapObject_Manager::GetInstance()->DestroyInstance();
    CGrid_Manager::GetInstance()->DestroyInstance();


}

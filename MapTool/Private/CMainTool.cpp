#include "CMainTool.h"
#include "CImGui_Manager.h"
#include "CGameInstance.h"


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

    if(FAILED(pGameInstance->Initialize_Engine(desc, &m_pDevice, &m_pContext)))
        return E_FAIL;

    pImGui_Manager->Init(g_hWnd, m_pDevice, m_pContext);

    return S_OK;
}

void CMainTool::Update(_float fTimeDelta)
{
    pImGui_Manager->Update();
}

void CMainTool::Render()
{
    pGameInstance->Draw_Begin(&ClearColor);
    pImGui_Manager->Render();
    pGameInstance->Draw_End();
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

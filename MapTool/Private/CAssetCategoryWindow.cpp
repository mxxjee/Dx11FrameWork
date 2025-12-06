#include "CAssetCategoryWindow.h"
#include "CMapObject.h"
#include "CGameInstance.h"
#include "CMapObject_Manager.h"
#include "CImgui_Button.h"
#include "CImgui_DataManager.h"

USING(MapTool)
CAssetCategoryWindow::CAssetCategoryWindow(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CImgui_Window(pDevice, pContext),
    pGameInstance{CGameInstance::GetInstance()},
    m_pMapObject_Manager{CMapObject_Manager::GetInstance()},
    m_pImgui_DataManager{CImgui_DataManager::GetInstance()}
{
    Safe_AddRef(pGameInstance);
    Safe_AddRef(m_pImgui_DataManager);


}


HRESULT CAssetCategoryWindow::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Create_Widgets()))
        return E_FAIL;

    return S_OK;
}

HRESULT CAssetCategoryWindow::Create_Widgets()
{
    //버튼 만들기
    CImgui_Button::ImguiButton_Desc ModelButtonDesc;
    ModelButtonDesc.Tag = "Model";
    ModelButtonDesc.Label = "Model";
    ModelButtonDesc.m_RelativePos = ImVec2(10, 50);
    ModelButtonDesc.callback = [this]()
    {
        m_pImgui_DataManager->Send_SelectedCategory("Model");
        m_pImgui_DataManager->Send_SelectedIdx(-1);
    };

    if (FAILED(Add_Widgets<CImgui_Button>(&ModelButtonDesc)))
        return E_FAIL;


    CImgui_Button::ImguiButton_Desc TileButtonDesc;
    TileButtonDesc.Tag = "Tile";
    TileButtonDesc.Label = "Tile";
    TileButtonDesc.m_RelativePos = ImVec2(10, 80);
    TileButtonDesc.callback = [this]()
    {
        m_pImgui_DataManager->Send_SelectedCategory("Tile");
        m_pImgui_DataManager->Send_SelectedIdx(-1);//초기화
    };

    if (FAILED(Add_Widgets<CImgui_Button>(&TileButtonDesc)))
        return E_FAIL;

    CImgui_Button::ImguiButton_Desc InteractionButtonDesc;
    InteractionButtonDesc.Tag = "Interaction";
    InteractionButtonDesc.Label = "Interaction";
    InteractionButtonDesc.m_RelativePos = ImVec2(10, 110);
    InteractionButtonDesc.callback = [this]()
    {
        m_pImgui_DataManager->Send_SelectedCategory("Interaction");
        m_pImgui_DataManager->Send_SelectedIdx(-1);//초기화

    };


    if (FAILED(Add_Widgets<CImgui_Button>(&InteractionButtonDesc)))
        return E_FAIL;

    return S_OK;
}


void CAssetCategoryWindow::Update()
{
    __super::Update();
}

void CAssetCategoryWindow::Render()
{
   
}

CAssetCategoryWindow* CAssetCategoryWindow::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg)
{
    CAssetCategoryWindow* pInstance = new CAssetCategoryWindow(pDevice, pContext);


    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Create : CAssetCategoryWindow");
        Safe_Release(pInstance);
    }


    return pInstance;
}

void CAssetCategoryWindow::Free()
{
    __super::Free();


    Safe_Release(m_pImgui_DataManager);
    Safe_Release(pGameInstance);
}

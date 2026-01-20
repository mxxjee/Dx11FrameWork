#include "CGridDebugWindow.h"
#include "CImgui_Widget.h"
#include "CImgui_InputInt.h"
#include "CGameInstance.h"

#include "CMapGrid.h"
#include "CVIBuffer_CustomTerrain.h"

#include "CImgui_Button.h"





USING(MapTool)
CGridDebugWindow::CGridDebugWindow(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CImgui_Window(pDevice, pContext)
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CGridDebugWindow::Initialize(void* pArg)
{
    m_InputTexts.resize(2);

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Create_Widgets()))
        return E_FAIL;

    return S_OK;
}

void CGridDebugWindow::Update()
{
    ImGui::Begin(m_WindowTitle.c_str(), &m_bOpen);

    for (auto& i : m_vWidgets)
        i->Update();



    ImGui::End();
}

void CGridDebugWindow::Render()
{
}

void CGridDebugWindow::Set_MapTerrain(Engine::CGameObject* pObj)
{
    if (pObj)
    {
        m_MapGrid = dynamic_cast<CMapGrid*>(pObj);
    }
}

HRESULT CGridDebugWindow::Create_Widgets()
{
    CImgui_InputInt::IMGUITEXTINPUT_DESC InputIntDesc;
    InputIntDesc.Label = "NumVerticesX";
    InputIntDesc.Tag = InputIntDesc.Label;
    InputIntDesc.m_RelativePos = ImVec2(10, 30);
    InputIntDesc.pData = &pNumVerticesX;


    if (FAILED(Add_Widgets<CImgui_InputInt>(&InputIntDesc, reinterpret_cast<CImgui_Widget**>(&m_InputTexts[0]))))
        return E_FAIL;


    InputIntDesc.Label = "NumVerticesZ";
    InputIntDesc.Tag = "NumVerticesZ";
    InputIntDesc.m_RelativePos = ImVec2(10, 50);
    InputIntDesc.pData = &pNumVerticesZ;

    if (FAILED(Add_Widgets<CImgui_InputInt>(&InputIntDesc, reinterpret_cast<CImgui_Widget**>(&m_InputTexts[1]))))
        return E_FAIL;


    CImgui_Button::ImguiButton_Desc ButtonDesc;
    ButtonDesc.Tag = "Apply";
    ButtonDesc.Label = "Apply";
    ButtonDesc.m_RelativePos = ImVec2(100, 100);
    ButtonDesc.callback = [this]()
    {
        m_MapGrid->Update_Terrain(pNumVerticesX, pNumVerticesZ);
    };

    if (FAILED(Add_Widgets<CImgui_Button>(&ButtonDesc, reinterpret_cast<CImgui_Widget**>(&m_pButton))))
        return E_FAIL;

    return S_OK;
}

void CGridDebugWindow::Init_NumValues()
{
    CheckNull(m_MapGrid);
    pNumVerticesX = m_MapGrid->Get_CustomVIBuffer()->Get_NumVerticesX();
    pNumVerticesZ = m_MapGrid->Get_CustomVIBuffer()->Get_NumVerticesZ();
}

CGridDebugWindow* CGridDebugWindow::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg)
{
    CGridDebugWindow* pInstance = new CGridDebugWindow(pDevice, pContext);


    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Create : CGridDebugWindow");
        Safe_Release(pInstance);
    }


    return pInstance;
}

void CGridDebugWindow::Free()
{
    __super::Free();
    for (auto& i : m_InputTexts)
        Safe_Release(i);

    Safe_Release(m_pButton);

    Safe_Release(m_pGameInstance);
}

#include "CTerrainDebugWindow.h"
#include "CImgui_Widget.h"
#include "CImgui_InputInt.h"
#include "CGameInstance.h"

#include "CMapTerrain.h"
#include "CVIBuffer_CustomTerrain.h"

#include "CImgui_Button.h"





USING(MapTool)
CTerrainDebugWindow::CTerrainDebugWindow(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CImgui_Window(pDevice, pContext)
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CTerrainDebugWindow::Initialize(void* pArg)
{
    m_InputTexts.resize(2);

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Create_Widgets()))
        return E_FAIL;

    return S_OK;
}

void CTerrainDebugWindow::Update()
{
    ImGui::Begin(m_WindowTitle.c_str(), &m_bOpen);

    for (auto& i : m_vWidgets)
        i->Update();



    ImGui::End();
}

void CTerrainDebugWindow::Render()
{
}

void CTerrainDebugWindow::Set_MapTerrain(CGameObject* pObj)
{
    if (pObj)
    {
        m_pMapTerrain = dynamic_cast<CMapTerrain*>(pObj);
    }
}

HRESULT CTerrainDebugWindow::Create_Widgets()
{
    CImgui_InputInt::IMGUITEXTINPUT_DESC InputIntDesc;
    InputIntDesc.Label = "NumVerticesX";
    InputIntDesc.Tag = InputIntDesc.Label;
    InputIntDesc.m_RelativePos = ImVec2(30, 30);
    InputIntDesc.pData = &pNumVerticesX;


    if (FAILED(Add_Widgets<CImgui_InputInt>(&InputIntDesc, reinterpret_cast<CImgui_Widget**>(&m_InputTexts[0]))))
        return E_FAIL;


    InputIntDesc.Label = "NumVerticesZ";
    InputIntDesc.Tag = "NumVerticesZ";
    InputIntDesc.m_RelativePos = ImVec2(30, 50);
    InputIntDesc.pData = &pNumVerticesZ;

    if (FAILED(Add_Widgets<CImgui_InputInt>(&InputIntDesc, reinterpret_cast<CImgui_Widget**>(&m_InputTexts[1]))))
        return E_FAIL;


    CImgui_Button::ImguiButton_Desc ButtonDesc;
    ButtonDesc.Tag = "Apply";
    ButtonDesc.Label = "Apply";
    ButtonDesc.m_RelativePos = ImVec2(50, 100);
    ButtonDesc.callback = [this]()
    {
        m_pMapTerrain->Update_Terrain(pNumVerticesX, pNumVerticesZ);
    };

    if (FAILED(Add_Widgets<CImgui_Button>(&ButtonDesc, reinterpret_cast<CImgui_Widget**>(&m_pButton))))
        return E_FAIL;

    return S_OK;
}

void CTerrainDebugWindow::Init_NumValues()
{
    CheckNull(m_pMapTerrain);
    pNumVerticesX = m_pMapTerrain->Get_CustomVIBuffer()->Get_NumVerticesX();
    pNumVerticesZ = m_pMapTerrain->Get_CustomVIBuffer()->Get_NumVerticesZ();
}

CTerrainDebugWindow* CTerrainDebugWindow::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg)
{
    CTerrainDebugWindow* pInstance = new CTerrainDebugWindow(pDevice, pContext);


    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Create : CTerrainDebugWindow");
        Safe_Release(pInstance);
    }


    return pInstance;
}

void CTerrainDebugWindow::Free()
{
    __super::Free();
    for (auto& i : m_InputTexts)
        Safe_Release(i);

    Safe_Release(m_pGameInstance);
}

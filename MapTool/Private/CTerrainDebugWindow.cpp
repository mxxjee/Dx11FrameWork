#include "CTerrainDebugWindow.h"
#include "CImgui_Widget.h"
#include "CImgui_InputText.h"
#include "CGameInstance.h"
#include "CMapTerrain.h"



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
    CImgui_InputText::IMGUITEXTINPUT_DESC InputTextDesc;
    InputTextDesc.Label = "NumVerticesX";
    InputTextDesc.Tag = InputTextDesc.Label;
    InputTextDesc.m_RelativePos = ImVec2(30, 30);



    if (FAILED(Add_Widgets<CImgui_InputText>(&InputTextDesc, reinterpret_cast<CImgui_Widget**>(&m_InputTexts[0]))))
        return E_FAIL;


    InputTextDesc.Label = "NumVerticesZ";
    InputTextDesc.Tag = InputTextDesc.Label;
    InputTextDesc.m_RelativePos = ImVec2(30, 50);

    if (FAILED(Add_Widgets<CImgui_InputText>(&InputTextDesc, reinterpret_cast<CImgui_Widget**>(&m_InputTexts[1]))))
        return E_FAIL;

    return S_OK;
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

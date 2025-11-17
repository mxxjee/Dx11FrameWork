#include "CNavMeshDebugWindow.h"
#include "CNavMeshEdit_Manager.h"
#include "CImgui_Button.h"
#include "CNavMeshEdit_Manager.h"

USING(MapTool)
CNavMeshDebugWindow::CNavMeshDebugWindow(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CImgui_Window(pDevice, pContext), m_pNavMeshEdit_Manager(CNavMeshEdit_Manager::GetInstance())
{
}



HRESULT CNavMeshDebugWindow::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Create_Widgets()))
        return E_FAIL;

    return S_OK;
}

HRESULT CNavMeshDebugWindow::Create_Widgets()
{
    CImgui_Button::ImguiButton_Desc ButtonDesc;
    ButtonDesc.Tag = "SetUp_Neighbor";
    ButtonDesc.Label = "SetUp_Neighbor";
    ButtonDesc.m_RelativePos = ImVec2(100, 100);
    ButtonDesc.callback = [this]()
    {
        CNavMeshEdit_Manager::GetInstance()->SetUp_Neighbors();
    };

    if (FAILED(Add_Widgets<CImgui_Button>(&ButtonDesc, reinterpret_cast<CImgui_Widget**>(&m_pButton))))
        return E_FAIL;

    return S_OK;
}
void CNavMeshDebugWindow::Update()
{
    ImGui::Begin(m_WindowTitle.c_str(), &m_bOpen);

    for (auto& i : m_vWidgets)
        i->Update();

    m_pNavMeshEdit_Manager->Render_Preview_Imgui_Render();
    ImGui::End();
}

void CNavMeshDebugWindow::Render()
{
}

void CNavMeshDebugWindow::UpdatePoints(deque<PreviewPoint> Points)
{
}

CNavMeshDebugWindow* CNavMeshDebugWindow::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg)
{
    CNavMeshDebugWindow* pInstance = new CNavMeshDebugWindow(pDevice, pContext);


    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Create : CLayerDebugWindow");
        Safe_Release(pInstance);
    }


    return pInstance;
}

void CNavMeshDebugWindow::Free()
{
    __super::Free();
    Safe_Release(m_pButton);

}

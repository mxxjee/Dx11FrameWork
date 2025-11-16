#include "CNavMeshDebugWindow.h"
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

    return S_OK;
}

void CNavMeshDebugWindow::Update()
{
    ImGui::Begin(m_WindowTitle.c_str(), &m_bOpen);
    m_pNavMeshEdit_Manager->Render_Preview_Imgui_Render();
    ImGui::End();
}

void CNavMeshDebugWindow::Render()
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


}

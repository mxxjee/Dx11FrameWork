#include "CImgui_Window.h"
#include "CImgui_Widget.h"


CImgui_Window::CImgui_Window(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CImgui_Base(pDevice,pContext)
{
}


HRESULT CImgui_Window::Initialize(void* pArg)
{

    IMGUIWINDOW_DESC* pDesc = static_cast<IMGUIWINDOW_DESC*>(pArg);
    m_WindowTitle = pDesc->m_WindowTitle;

    m_vWidgets = pDesc->m_Widgets;
    
    ImGui::SetNextWindowPos(pDesc->m_WindowPos, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(pDesc->m_WindowSize, ImGuiCond_FirstUseEver);
    

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;


    return S_OK;
}

void CImgui_Window::Update_Priority()
{
}

void CImgui_Window::Update()
{
    
    if (!m_bOpen || !m_bActive)
        return;

    ImGui::Begin(m_WindowTitle.c_str(),&m_bOpen);
   
    for (auto& i : m_vWidgets)
        i->Update();

    ImGui::End();
}

void CImgui_Window::Render()
{
    for (auto& i : m_vWidgets)
        i->Render();
}

CImgui_Window* CImgui_Window::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg)
{
    CImgui_Window* pInstance = new CImgui_Window(pDevice, pContext);


    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Create : Imgui_Window");
        Safe_Release(pInstance);
    }


    return pInstance;
}

void CImgui_Window::Free()
{
    __super::Free();
    for (auto& i : m_vWidgets)
        Safe_Release(i);
}

#include "CImgui_Window.h"

CImgui_Window::CImgui_Window()
    :CImgui_Base()
{
}

CImgui_Window::CImgui_Window(const string& Tag)
    :CImgui_Base(Tag)
{
}

HRESULT CImgui_Window::Initialize()
{
    return S_OK;
}

void CImgui_Window::Update()
{
    ImGui::Begin(m_Tag.c_str());


    ImGui::End();
}

void CImgui_Window::Render()
{
}

void CImgui_Window::Free()
{
}

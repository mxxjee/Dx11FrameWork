#include "CImgui_Widget.h"

CImgui_Widget::CImgui_Widget(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CImgui_Base(pDevice,pContext)
{
}

HRESULT CImgui_Widget::Initialize(void* pArg)
{
	IMGUIWIDGET_DESC* pDesc = static_cast<IMGUIWIDGET_DESC*>(pArg);
	RelativePos = pDesc->m_RelativePos;

	
	return S_OK;
}

void CImgui_Widget::Update()
{
	ImGui::SetCursorScreenPos(RelativePos);
}

void CImgui_Widget::Render()
{
}

void CImgui_Widget::Free()
{
	__super::Free();

}

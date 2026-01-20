#include "CImgui_Button.h"

CImgui_Button::CImgui_Button(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CImgui_Widget(pDevice,pContext)
{
}

HRESULT CImgui_Button::Initialize(void* pArg)
{
	ImguiButton_Desc* pDesc = static_cast<ImguiButton_Desc*>(pArg);
	
	m_Label = pDesc->Label;
	m_Callback = pDesc->callback;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;


	return S_OK;
}

void CImgui_Button::Update()
{
	__super::Update();
	CheckFalse(m_bActive);

	if (ImGui::Button(m_Label.c_str()))
	{
		if (m_Callback)
			m_Callback();
	}
}

void CImgui_Button::Render()
{
	CheckFalse(m_bActive);
	__super::Render();

}

CImgui_Button* CImgui_Button::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg)
{
	CImgui_Button* pInstance = new CImgui_Button(pDevice, pContext);


	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Create : Imgui_Button");
		Safe_Release(pInstance);
	}


	return pInstance;
}

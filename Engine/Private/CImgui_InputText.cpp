#include "CImgui_InputText.h"

CImgui_InputText::CImgui_InputText(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CImgui_Widget(pDevice,pContext)
{
}

HRESULT CImgui_InputText::Initialize(void* pArg)
{
	CImgui_InputText::ImguiTextInputDesc* pDesc = static_cast<CImgui_InputText::ImguiTextInputDesc*>(pArg);

	m_Label = pDesc->Label;
	m_Callback = pDesc->callback;


	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;


	return S_OK;
}

void CImgui_InputText::Update()
{
	__super::Update();

	if (ImGui::InputText(m_Label.c_str(), buf, IM_ARRAYSIZE(buf)))
	{
		// 사용자가 입력을 수정할 때마다 true 리턴됨
		if (m_Callback)
			m_Callback();
	}
}

void CImgui_InputText::Render()
{
}

CImgui_InputText* CImgui_InputText::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg)
{
	CImgui_InputText* pInstance = new CImgui_InputText(pDevice, pContext);


	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Create : CImgui_InputText");
		Safe_Release(pInstance);
	}


	return pInstance;
}

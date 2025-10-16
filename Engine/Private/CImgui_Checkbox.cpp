#include "CImgui_Checkbox.h"

CImgui_Checkbox::CImgui_Checkbox(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CImgui_Widget(pDevice, pContext)
{
}

HRESULT CImgui_Checkbox::Initialize(void* pArg)
{
	ImguiCheckbox_Desc* pDesc = static_cast<ImguiCheckbox_Desc*>(pArg);

	m_Label = pDesc->Label;
	m_Callback = pDesc->callback;
	m_bValue = pDesc->bValue;


	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;


	return S_OK;
}

void CImgui_Checkbox::Update()
{
	CheckFalse(m_bActive);
	__super::Update();

	if (ImGui::Checkbox(m_Label.c_str(),m_bValue))
	{
		if (m_Callback)
			m_Callback();
	}

}

void CImgui_Checkbox::Render()
{
	CheckFalse(m_bActive);
	__super::Render();
}

CImgui_Checkbox* CImgui_Checkbox::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg)
{
	CImgui_Checkbox* pInstance = new CImgui_Checkbox(pDevice, pContext);


	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Create : CImgui_Checkbox");
		Safe_Release(pInstance);
	}


	return pInstance;
}

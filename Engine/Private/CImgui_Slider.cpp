#include "CImgui_Slider.h"

CImgui_Slider::CImgui_Slider(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CImgui_Widget(pDevice,pContext)
{
}

HRESULT CImgui_Slider::Initialize(void* pArg)
{
	ImguiSlider_Desc* pDesc = static_cast<ImguiSlider_Desc*>(pArg);

	m_Label = pDesc->m_LabelName;
	m_fBindValue = pDesc->fValue;

	m_vMin = pDesc->vMin;
	m_vMax = pDesc->vMax;

	m_Callback = pDesc->callback;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;



	return S_OK;
}

void CImgui_Slider::Update()
{
	__super::Update();

	CheckNull(m_fBindValue);
	//CheckFalse(m_bActive);

	
	


	if (ImGui::SliderFloat(m_Label.c_str(), m_fBindValue, m_vMin,m_vMax))
	{
		if (m_Callback)
			m_Callback();

	}
}

void CImgui_Slider::Render()
{
	CheckNull(m_fBindValue);
	__super::Render();
}

CImgui_Slider* CImgui_Slider::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg)
{

	CImgui_Slider* pInstance = new CImgui_Slider(pDevice, pContext);


	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Create : Imgui_Slider");
		Safe_Release(pInstance);
	}


	return pInstance;
}

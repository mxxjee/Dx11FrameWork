#include "CImgui_InputFloat.h"

CImgui_InputFloat::CImgui_InputFloat(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CImgui_Widget(pDevice, pContext)
{
}

HRESULT CImgui_InputFloat::Initialize(void* pArg)
{
	CImgui_InputFloat::ImguiTextFloatDesc* pDesc = static_cast<CImgui_InputFloat::ImguiTextFloatDesc*>(pArg);

	m_Label = pDesc->Label;
	m_Callback = pDesc->callback;
	m_pData = pDesc->pData;
	m_Step = pDesc->Step;


	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;


	return S_OK;
}

void CImgui_InputFloat::Update()
{

	CheckFalse(m_bActive);
	__super::Update();


	if (m_pData)
	{
		if (ImGui::InputFloat(m_Label.c_str(), m_pData, m_Step))
		{
			// 사용자가 입력을 수정할 때마다 true 리턴됨
			if (m_Callback)
				m_Callback();
		}

	}
}

void CImgui_InputFloat::Render()
{
}

CImgui_InputFloat* CImgui_InputFloat::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg)
{
	CImgui_InputFloat* pInstance = new CImgui_InputFloat(pDevice, pContext);


	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Create : CImgui_InputFloat");
		Safe_Release(pInstance);
	}


	return pInstance;
}

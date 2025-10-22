#include "CImgui_InputInt.h"

CImgui_InputInt::CImgui_InputInt(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CImgui_Widget(pDevice,pContext)
{
}

HRESULT CImgui_InputInt::Initialize(void* pArg)
{
	CImgui_InputInt::ImguiTextInputDesc* pDesc = static_cast<CImgui_InputInt::ImguiTextInputDesc*>(pArg);

	m_Label = pDesc->Label;
	m_Callback = pDesc->callback;
	m_pData = pDesc->pData;
	m_Step = pDesc->Step;


	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	
	return S_OK;
}

void CImgui_InputInt::Update()
{
	__super::Update();

	if (m_pData)
	{
		if (ImGui::InputInt(m_Label.c_str(), m_pData, m_Step))
		{
			// 사용자가 입력을 수정할 때마다 true 리턴됨
			if (m_Callback)
				m_Callback();
		}

	}
}

void CImgui_InputInt::Render()
{
}

CImgui_InputInt* CImgui_InputInt::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg)
{
	CImgui_InputInt* pInstance = new CImgui_InputInt(pDevice, pContext);


	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Create : CImgui_InputInt");
		Safe_Release(pInstance);
	}


	return pInstance;
}

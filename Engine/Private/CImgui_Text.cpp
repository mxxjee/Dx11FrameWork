#include "CImgui_Text.h"

CImgui_Text::CImgui_Text(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CImgui_Widget(pDevice,pContext)
{
}

HRESULT CImgui_Text::Initialize(void* pArg)
{
	ImguiText_Desc* pDesc = static_cast<ImguiText_Desc*>(pArg);

	m_Callback = pDesc->TextCallback;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;


    return S_OK;
}

void CImgui_Text::Update()
{
	__super::Update();
	if (m_Callback)
		ImGui::Text("%s", m_Callback().c_str());


}

void CImgui_Text::Render()
{
	__super::Render();
}

CImgui_Text* CImgui_Text::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg)
{
	CImgui_Text* pInstance = new CImgui_Text(pDevice, pContext);


	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Create : Imgui_Text");
		Safe_Release(pInstance);
	}


	return pInstance;
}

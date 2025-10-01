#include "CImgui_Base.h"


CImgui_Base::CImgui_Base(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:m_pDevice(pDevice),m_pContext(pContext)
{
}

HRESULT CImgui_Base::Initialize(void* pArg)
{
	IMGUI_DESC* pDesc = static_cast<IMGUI_DESC*>(pArg);
	m_Tag = pDesc->Tag;

	return S_OK;
}

void CImgui_Base::Free()
{
	__super::Free();
}

#include "CLevel_Editor.h"
#include "Client_Defines.h"

USING(MapTool)

CLevel_Editor::CLevel_Editor(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
	:CLevel(_pDevice,_pDeviceContext)
{
}

HRESULT CLevel_Editor::Initialize(LevelArgs& args)
{
	__super::Initialize(args);

	return S_OK;
}

void CLevel_Editor::Update_Priority(_float fTimeDelta)
{
}

void CLevel_Editor::Update(const _float fTimeDelta)
{
}

void CLevel_Editor::Update_Late(_float fTimeDelta)
{
}

void CLevel_Editor::Render()
{
}

HRESULT CLevel_Editor::Ready_Layer_Enviroment(const _wstring& strLayerTag)
{
	return E_NOTIMPL;
}

HRESULT CLevel_Editor::Ready_Layer_UI(const _wstring& strLayerTag)
{
	return E_NOTIMPL;
}

HRESULT CLevel_Editor::Ready_Layer_MainCamera(const _wstring& strLayerTag)
{
	return E_NOTIMPL;
}

HRESULT CLevel_Editor::Ready_Layer_Player(const _wstring& strLayerTag)
{
	return E_NOTIMPL;
}

void CLevel_Editor::OnEnter()
{
	SetWindowText(g_hWnd, L"∏ ≈¯ æ¿ ¿‘¥œ¥Ÿ.");
}

void CLevel_Editor::OnResume()
{
}

void CLevel_Editor::OnPause()
{
}

void CLevel_Editor::OnExit()
{
}

CLevel_Editor* CLevel_Editor::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext, LevelArgs& args)
{
	CLevel_Editor* pInstance = new CLevel_Editor(_pDevice, _pDeviceContext);
	if (FAILED(pInstance->Initialize(args)))
	{
		MSG_BOX("Failed to Create : CLevel_Editor");
		Safe_Release(pInstance);
	}


	return pInstance;
}

void CLevel_Editor::Free()
{
	__super::Free();
}

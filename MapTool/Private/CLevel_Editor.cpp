#include "CLevel_Editor.h"
#include "Client_Defines.h"

#include "CGameInstance.h"

#include "CFreeCamera.h"
#include "CPerspectiveCameraComponent.h"


USING(MapTool)

CLevel_Editor::CLevel_Editor(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
	:CLevel(_pDevice,_pDeviceContext)
{
}

HRESULT CLevel_Editor::Initialize(LevelArgs& args)
{
	__super::Initialize(args);

	if (FAILED(Ready_Layer_MainCamera(L"Camera_Layer")))
		return E_FAIL;

	return S_OK;
}

void CLevel_Editor::Update_Priority(_float fTimeDelta)
{
	__super::Update_Priority(fTimeDelta);
}

void CLevel_Editor::Update(const _float fTimeDelta)
{
	__super::Update(fTimeDelta);
}

void CLevel_Editor::Update_Late(_float fTimeDelta)
{
	__super::Update_Late(fTimeDelta);
}

void CLevel_Editor::Render()
{
	__super::Render();
	SetWindowText(g_hWnd, L"∏ ≈¯ æ¿ ¿‘¥œ¥Ÿ.");
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
	CFreeCamera::GAMEOBJECT_DESC Desc = {};
	Desc.ObjTag = L"FreeCamera";

	CTransform::TRANSFORM_DESC TransDesc = {};
	TransDesc.fRotationPerSec = 10.f;
	TransDesc.fSpeedPerSec = 5.f;
	TransDesc.vLocalRotation = { 30.f,0.f,0.f,1.f };

	CPerspectiveCameraComponent::PERSPECTIVE_DESC CameraDesc = {};
	CameraDesc.Aspect = (float)g_iWinSizeX / g_iWinSizeY;
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 1000.f;

	Desc.CameraDesc = &CameraDesc;
	Desc.TransformDesc = &TransDesc;


	CGameObject* pInstance = dynamic_cast<CGameObject*>(
		m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_OBJ_NAME(L"FreeCamera"), &Desc));
	
	m_pGameInstance->RegisterCamera(L"FreeCamera", pInstance, false);

	return S_OK;
}

HRESULT CLevel_Editor::Ready_Layer_Player(const _wstring& strLayerTag)
{
	return E_NOTIMPL;
}

void CLevel_Editor::OnEnter()
{
	m_pGameInstance->SetMainPerspectiveCamera(L"FreeCamera");
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

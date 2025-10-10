#include "CLevel_UI.h"
#include "CScreenQuad.h"
#include "CGameInstance.h"

CLevel_UI::CLevel_UI(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
	:CLevel(_pDevice, _pDeviceContext)
{
}

HRESULT CLevel_UI::Initialize(LevelArgs& args)
{
	__super::Initialize(args);

	if (FAILED(Ready_Layer_UI(L"UI_Layer")))
		return E_FAIL;

	return S_OK;
}

void CLevel_UI::Update_Priority(_float fTimeDelta)
{
	__super::Update_Priority(fTimeDelta);
	if (m_pGameInstance->IsKeyPressed(KeyCode::Space))
		m_pGameInstance->Pop_Level() ;

}

void CLevel_UI::Update(const _float fTimeDelta)
{
	__super::Update(fTimeDelta);


}

void CLevel_UI::Update_Late(_float fTimeDelta)
{
	__super::Update_Late(fTimeDelta);

}

void CLevel_UI::Render()
{
	//UI렌더
	SetWindowText(g_hWnd, L"UIOverlay 씬입니다.");
}

void CLevel_UI::OnEnter()
{
	//뒤에화면을 블러처리한 텍스처로 복사
	CGameObject* pObj = m_pGameInstance->Find_GameObject(ENUM_TO_UINT(LEVEL_ID::UI),
		L"UI_Layer", L"ScreenQuad");

	if (pObj)
	{
		CScreenQuad* pScreenQuad = dynamic_cast<CScreenQuad*>(pObj);
		if (pScreenQuad)
			pScreenQuad->Set_ScreenTexture(0);
	}
}

void CLevel_UI::OnResume()
{
}

void CLevel_UI::OnPause()
{
}

void CLevel_UI::OnExit()
{
}

CLevel_UI* CLevel_UI::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext, LevelArgs& args)
{

	CLevel_UI* pInstance = new CLevel_UI(_pDevice, _pDeviceContext);
	if (FAILED(pInstance->Initialize(args)))
	{
		MSG_BOX("Failed to Create : CLevel_UI");
		Safe_Release(pInstance);
	}


	return pInstance;
}

void CLevel_UI::Free()
{
	__super::Free();
}

HRESULT CLevel_UI::Ready_Layer_UI(const _wstring& strLayerTag)
{
	CScreenQuad::tagGameObjectDesc        Desc = {};

	Desc.ObjTag = L"ScreenQuad";
	

	CTransform::TRANSFORM_DESC TransDesc = {};
	TransDesc.vLocalScale = { g_iWinSizeX,g_iWinSizeY,1.f,1.f };

	Desc.TransformDesc = &TransDesc;

	if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVEL_ID::STATIC),
		PROTO_OBJ_NAME(L"ScreenQuad"),
		ENUM_TO_UINT(LEVEL_ID::UI),
		strLayerTag, &Desc)))
		return E_FAIL;

	return S_OK;
}

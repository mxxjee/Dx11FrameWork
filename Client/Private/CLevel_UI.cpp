#include "CLevel_UI.h"
#include "CScreenQuad.h"
#include "CGameInstance.h"
#include "CInput_Manager.h"


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
	if (CInput_Manager::GetInstance()->IsKeyPressed(KeyCode::Space))
		m_pGameInstance->Pop_Level() ;

	/*if (CInput_Manager::GetInstance()->IsKeyPressed(KeyCode::P))
		m_pGameInstance->SaveTextureToFile(L"RenderBehind", L"../../ScreenShots/1.png");*/

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
	m_pGameInstance->ScreenShot(L"RenderBehind");

	//뒤에화면을 블러처리한 텍스처로 복사
	CGameObject* pObj = m_pGameInstance->Find_GameObject(ENUM_TO_UINT(LEVEL_ID::UI),
		L"UI_Layer", L"ScreenQuad");

	if (pObj)
	{
		CTexture* pTexture = m_pGameInstance->Find_ScreenTexture(L"RenderBehind");

		CScreenQuad* pScreenQuad = dynamic_cast<CScreenQuad*>(pObj);
		if (pScreenQuad && pTexture)
			pScreenQuad->Make_ScreenShot(pTexture);

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

	CUI::tagUIDesc        Desc = {};
	
	Desc.eRenderGroup = ENUM_TO_UINT(RENDERGROUP::UI);
	Desc.ObjTag = L"ScreenQuad";
	
	Desc.Depth = 0.6f;
	Desc.fSizeX = g_iWinSizeX;
	Desc.fSizeY = g_iWinSizeY;

	Desc.fX = g_iWinSizeX>>1;
	Desc.fY = g_iWinSizeY>>1;

	Desc.passName = "Blur";


	Desc.Depth = 0.6f;

	CTransform::TRANSFORM_DESC TransDesc = {};
	Desc.TransformDesc = &TransDesc;

	if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(ENUM_TO_UINT(LEVEL_ID::STATIC),
		PROTO_OBJ_NAME(L"ScreenQuad"),
		ENUM_TO_UINT(LEVEL_ID::UI),
		strLayerTag, &Desc)))
		return E_FAIL;

	return S_OK;
}

#include "CLevel_Editor.h"
#include "Client_Defines.h"

#include "CGameInstance.h"

#include "CFreeCamera.h"
#include "CMapTerrain.h"

#include "CMapObject_Manager.h"
#include "CMapQuad.h"

#include "CImGui_Manager.h"
#include "CImgui_Base.h"
#include "CTerrainDebugWindow.h"

#include "CVIBuffer_Triangle.h"
#include "CTerrain_Highlight.h"



USING(MapTool)

CLevel_Editor::CLevel_Editor(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
	:CLevel(_pDevice, _pDeviceContext), m_pMapObject_Manager(CMapObject_Manager::GetInstance())
{
	Safe_AddRef(m_pMapObject_Manager);

}

HRESULT CLevel_Editor::Initialize(LevelArgs& args)
{
	__super::Initialize(args);

	if (FAILED(Ready_Layer_MainCamera(L"Camera_Layer")))
		return E_FAIL;


	if (FAILED(Ready_Layer_Enviroment(L"Enviroment_Layer")))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player(L"Player_Layer")))
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
	Triangle* pPickingPos= m_pGameInstance->PickTerrain(L"MapTerrain");
	if (pPickingPos!=nullptr)
	{
		if (FAILED(Create_TerrainHighlight(pPickingPos)))
			return;
	}
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

HRESULT CLevel_Editor::Create_TerrainHighlight(Triangle* PickingPos)
{
	CheckNullResult(PickingPos,E_FAIL);
	CMapTerrain* pMapTerrain = dynamic_cast<CMapTerrain*>(m_pGameInstance->Find_Terrain(L"MapTerrain"));
	CheckNullResult(pMapTerrain, E_FAIL);

	CheckTrueResult(pMapTerrain->CheckDuplication(PickingPos),E_FAIL);

	CTerrain_Highlight::HIGHLIGHT_DESC Desc;
	Desc.eRenderGroup = ENUM_TO_UINT(RENDERGROUP::NONALPHA);
	Desc.ObjTag = L"Terrain_Highlight" + to_wstring(m_iIdx);
	Desc.pOwner = m_pGameInstance->Find_Terrain(L"MapTerrain");
	Desc.ShaderName = L"VtxPosCor";
	Desc.passName = "Default";
	Desc.ObjType = MapObjType::TILE;



	CTransform::TRANSFORM_DESC transform;
	Desc.TransformDesc = &transform;

	CVIBuffer_Triangle::TRIANGLEBUFFER_DESC triangle;
	triangle.v0 = PickingPos->v0;
	triangle.v1 = PickingPos->v1;
	triangle.v2 = PickingPos->v2;

	Desc.TriangleBuffer = &triangle;

	CTerrain_Highlight* pTerrain_Highlight = dynamic_cast<CTerrain_Highlight*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT,
		ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_OBJ_NAME(L"Terrain_Highlight"), &Desc));

	if (pTerrain_Highlight)
	{
		if (FAILED(m_pMapObject_Manager->Add_MapObject_To_MapLayer(L"Highlight_Layer", pTerrain_Highlight)))
			return E_FAIL;

		
		pMapTerrain->Add_TerrainHighlights(pTerrain_Highlight);
	}



	++m_iIdx;
	return S_OK;
}

HRESULT CLevel_Editor::Ready_Layer_Enviroment(const _wstring& strLayerTag)
{
	/////////////////////////////////////
	CMapTerrain::TERRAIN_DESC pDesc;
	pDesc.TextureKey = L"Terrain";
	pDesc.ObjTag = L"MapTerrain";
	pDesc.ShaderName = L"VtxNorTex";
	pDesc.passName = "Default";
	pDesc.eRenderGroup = ENUM_TO_UINT(RENDERGROUP::PRIORITY);


	CTransform::TRANSFORM_DESC TransDesc = {};

	TransDesc.fRotationPerSec = 0.f;
	TransDesc.fSpeedPerSec = 1.f;


	TransDesc.vLocalScale = { 1.f,1.f,1.f,1.f };

	pDesc.TransformDesc = &TransDesc;

	CBase* pTerrain = m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_OBJ_NAME(L"MapTerrain"), &pDesc);
	if (pTerrain)
	{
		CTerrain_Base* ppTerrain = dynamic_cast<CTerrain_Base*>(pTerrain);
		if (ppTerrain)
			m_pGameInstance->Register_Terrain(L"MapTerrain", ppTerrain);
	}


	return S_OK;
}

HRESULT CLevel_Editor::Ready_Layer_UI(const _wstring& strLayerTag)
{
	return E_NOTIMPL;
}

HRESULT CLevel_Editor::Ready_Layer_MainCamera(const _wstring& strLayerTag)
{
	CCamera_Base::CAMERABASE_DESC Desc = {};
	Desc.ObjTag = L"FreeCamera";

	Desc.eCameraType = CAMERA_TYPE::FREE;
	Desc.eCameraFlag = CAMERA_FLAG::NONE;
	Desc.fWidth = (float)g_iWinSizeX;
	Desc.fHeight = (float)g_iWinSizeY;
	Desc.fNear = 0.1f;
	Desc.vPosition = _float3(0.f, 5.f, -5.f);
	Desc.vAt = { 0.f,5.f, 0.f};
	Desc.fFar = 1000.f;

	CTransform::TRANSFORM_DESC TransDesc = {};
	TransDesc.fRotationPerSec = 10.f;
	TransDesc.fSpeedPerSec = 5.f;
	


	Desc.TransformDesc = &TransDesc;


	CGameObject* pInstance = dynamic_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_OBJ_NAME(L"FreeCamera"), &Desc));
	m_pGameInstance->RegisterCamera(CAMERA_TYPE::FREE, pInstance);


	return S_OK;
}

HRESULT CLevel_Editor::Ready_Layer_Player(const _wstring& strLayerTag)
{
	CMapQuad::MAPQUAD_DESC Desc;
	Desc.eRenderGroup = ENUM_TO_UINT(RENDERGROUP::ALPHA);
	Desc.ObjTag = L"Test_Quad";
	Desc.TextureKey = L"Keroro";

	Desc.ShaderName = L"Default";
	Desc.passName = "Default";
	Desc.ObjType = MapObjType::MODEL;



	CTransform::TRANSFORM_DESC transform;
	Desc.TransformDesc = &transform;
	
	if (FAILED(m_pMapObject_Manager->Add_MapObject_To_MapLayer(ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_OBJ_NAME(L"MapQuad"),
		strLayerTag, &Desc)))
		return E_FAIL;





	return S_OK;
}

void CLevel_Editor::OnEnter()
{
	CImgui_Base* pWindow = CImGui_Manager::GetInstance()->Find_Window("TerrainDebugWindow");
	if (pWindow)
	{
		CTerrainDebugWindow* pTerrainDebugWindow = dynamic_cast<CTerrainDebugWindow*>(pWindow);
		if (pTerrainDebugWindow)
		{
			pTerrainDebugWindow->Set_MapTerrain(m_pGameInstance->Find_Terrain(L"MapTerrain"));
			pTerrainDebugWindow->Init_NumValues();

		}

	}
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

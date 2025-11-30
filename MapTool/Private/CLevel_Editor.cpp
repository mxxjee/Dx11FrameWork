#include "CLevel_Editor.h"
#include "Client_Defines.h"

#include "CGameInstance.h"

#include "CFreeCamera.h"
#include "CMapGrid.h"

#include "CMapObject_Manager.h"
#include "CMapQuad.h"

#include "CImGui_Manager.h"
#include "CImgui_Base.h"
#include "CGridDebugWindow.h"

#include "CVIBuffer_Triangle.h"
#include "CTerrain_Highlight.h"

#include "CMapLayer.h"
#include "CGrid_Manager.h"
#include "CMapTerrain.h"


#include "CImgui_Window.h"
#include "CImGui_Manager.h"
#include "CAssetListWindow.h"

#include "CMapModel.h"
#include "CInput_Manager.h"
#include "CModel.h"

#include "CNavMeshEdit_Manager.h"
#include "CNavEditPreview.h"

USING(MapTool)

CLevel_Editor::CLevel_Editor(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
	:CLevel(_pDevice, _pDeviceContext), 
	m_pMapObject_Manager(CMapObject_Manager::GetInstance()),
	m_pImgui_Manager(CImGui_Manager::GetInstance())

{
	Safe_AddRef(m_pImgui_Manager);


}

HRESULT CLevel_Editor::Initialize(LevelArgs& args)
{
	__super::Initialize(args);

	m_pGrid_Manager = CGrid_Manager::GetInstance();

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




	


}

void CLevel_Editor::Update_Late(_float fTimeDelta)
{
	__super::Update_Late(fTimeDelta);
	switch (m_pImgui_Manager->Get_MapToolMode())
	{
	case MapToolMode::EDIT:
		Terrain_Picking();
		break;

	case MapToolMode::NAVMESH:
		Terrain_Picking_WorldPos();
		break;

	}
}

void CLevel_Editor::Render()
{
	__super::Render();
	SetWindowText(g_hWnd, L"맵툴 씬 입니다.");
}

HRESULT CLevel_Editor::Create_TerrainHighlight()
{

	/*CMapTerrain* pMapTerrain = dynamic_cast<CMapTerrain*>(m_pGameInstance->Find_Terrain(L"MapTerrain"));
	CheckNullResult(pMapTerrain, E_FAIL);

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

		CMapLayer* pLayer = m_pMapObject_Manager->Find_MapLayer(L"Highlight_Layer");
		if (pLayer)
			pLayer->Set_AblePicking(false);

		pMapTerrain->Add_TerrainHighlights(pTerrain_Highlight);
	}
*/

	return S_OK;
}

HRESULT CLevel_Editor::Ready_Layer_Enviroment(const _wstring& strLayerTag)
{
	/////////////////////////////////////
	CMapGrid::TERRAIN_DESC pDesc;
	pDesc.TextureKey = L"";
	pDesc.ObjTag = L"MapGrid";


	pDesc.ShaderName = L"Default";
	pDesc.passName = "Default";
	pDesc.eRenderGroup = ENUM_TO_UINT(RENDERGROUP::PRIORITY);


	CTransform::TRANSFORM_DESC TransDesc = {};

	TransDesc.fRotationPerSec = 0.f;
	TransDesc.fSpeedPerSec = 1.f;


	TransDesc.vLocalScale = { 1.f,1.f,1.f,1.f };

	pDesc.TransformDesc = &TransDesc;

	CGameObject* pMapGrid =dynamic_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_OBJ_NAME(L"MapGrid"), &pDesc));
	if (pMapGrid)
		m_pGrid_Manager->Set_MainGrid(pMapGrid);



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
	Desc.fFar = 500.f;

	CTransform::TRANSFORM_DESC TransDesc = {};
	TransDesc.fRotationPerSec = 10.f;
	TransDesc.fSpeedPerSec = 8.f;
	


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
	Desc.ObjType = MapObjType::OBSTACLE;
	


	CTransform::TRANSFORM_DESC transform;
	transform.vLocalRotation = _float4(90.f, 0.f, 0.f, 1.f);

	Desc.TransformDesc = &transform;
	
	if (FAILED(m_pMapObject_Manager->Add_MapObject_To_MapLayer(ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_OBJ_NAME(L"MapQuad"),
		strLayerTag, &Desc)))
		return E_FAIL;





	return S_OK;
}

void CLevel_Editor::Terrain_Picking()
{

	Triangle* pPickingPos = m_pGrid_Manager->PickGrid();
	m_pGrid_Manager->Set_MouseWorldPos();

	CTerrain_Base* pBase = m_pGameInstance->Get_PickTerrain();
	if (pBase)
	{
		CMapTerrain* ppTerrain = dynamic_cast<CMapTerrain*>(pBase);
		if (ppTerrain)
		{
			m_pMapObject_Manager->Set_SelectObject(ppTerrain);
			m_pMapObject_Manager->Set_AblePicking(false);
		}

	}
	else
	{/*
		if (m_pMapObject_Manager->Get_SelectObject() != nullptr && m_pMapObject_Manager->Get_SelectObject()->Get_ObjType()==MapObjType::TERRAIN)
			m_pMapObject_Manager->Set_SelectObject(nullptr);*/
	}

}

void CLevel_Editor::Terrain_Picking_WorldPos()
{
	ImGuiIO& io = ImGui::GetIO();
	CheckTrue(io.WantCaptureMouse);

	CheckTrue(m_pGameInstance->Get_PickTerrain() == nullptr);

	//삼각형Fix상태
	CNavMeshEdit_Manager::GetInstance()->Set_FixCell(CInput_Manager::GetInstance()->IsKeyHeld(KeyCode::LShift));
	CNavMeshEdit_Manager::GetInstance()->Set_FixEdge(CInput_Manager::GetInstance()->IsKeyHeld(KeyCode::Alt));
	//Tab.누르면 네이버생성
	if (CInput_Manager::GetInstance()->IsKeyPressed(KeyCode::Tab))
		CNavMeshEdit_Manager::GetInstance()->SetUp_Neighbors();

	

	if (CInput_Manager::GetInstance()->IsKeyHeld(KeyCode::LControl))
	{
		if(CInput_Manager::GetInstance()->IsKeyPressed(KeyCode::Z))
			CNavMeshEdit_Manager::GetInstance()->Ctrl_Z();

	}

	if(CInput_Manager::GetInstance()->IsKeyPressed(KeyCode::C))
		CNavMeshEdit_Manager::GetInstance()->Clear_Points();
	else
	{

		//그냥 이어찍기..
		_float3 vTerrainPickingPos = m_pGameInstance->Get_PickingWorldPos();
		vTerrainPickingPos.y += 0.05f;
		CNavMeshEdit_Manager::GetInstance()->Set_DrawPoint(vTerrainPickingPos, CInput_Manager::GetInstance()->IsMouseButtonPressed(0));


	
		
	}

}


void CLevel_Editor::OnEnter()
{
	m_pGameInstance->Set_MainCamera(CAMERA_TYPE::FREE);

	CImgui_Base* pWindow = CImGui_Manager::GetInstance()->Find_Window("GridDebugWindow");
	if (pWindow)
	{
		CGridDebugWindow* pGridDebugWindow = dynamic_cast<CGridDebugWindow*>(pWindow);
		if (pGridDebugWindow)
		{

			CMapGrid* pObj = m_pGrid_Manager->Get_MainGrid();
			
			if (pObj)
			{
				pGridDebugWindow->Set_MapTerrain(pObj);
				pGridDebugWindow->Init_NumValues();

			}

		}

	}

	//

	CImgui_Base* ppWindow = CImGui_Manager::GetInstance()->Find_Window("AssetList");
	if (ppWindow)
	{
		CAssetListWindow* pAssetListWindow = dynamic_cast<CAssetListWindow*>(ppWindow);
		if (pAssetListWindow)
		{
			if (FAILED(pAssetListWindow->Set_AssetList()))
				return;
		}
	}

	CNavMeshEdit_Manager::GetInstance()->Initialize(m_pDevice,m_pContext);

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
	Safe_Release(m_pGrid_Manager);
	Safe_Release(m_pImgui_Manager);
	Safe_Release(m_pMapObject_Manager);


}

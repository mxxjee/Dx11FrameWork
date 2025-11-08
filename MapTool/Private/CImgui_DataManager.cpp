#include "CImgui_DataManager.h"
#include "MapTool_Defines.h"
#include "CInput_Manager.h"
#include "CMapObject_Manager.h"
#include "CMapQuad.h"
#include "Client_Defines.h"
#include "CGameInstance.h"

#include "CTerrain_Manager.h"
#include "CGrid_Manager.h"
#include "CMapModel.h"
#include "CModel.h"

#include "CGameObject.h"

#include "CMapTerrain.h"
#include "CMapLayer.h"


USING(MapTool)

IMPLEMENT_SINGLETON(CImgui_DataManager)

CImgui_DataManager::CImgui_DataManager()
	:m_pInputManager(CInput_Manager::GetInstance()),
	m_pMapObject_Manager(CMapObject_Manager::GetInstance()),
	m_pGameInstance(CGameInstance::GetInstance()),
	m_pGrid_Manager(CGrid_Manager::GetInstance())
{
	Safe_AddRef(m_pMapObject_Manager);
	Safe_AddRef(m_pGameInstance);
	Safe_AddRef(m_pInputManager);
	Safe_AddRef(m_pGrid_Manager);
}

CImgui_DataManager::~CImgui_DataManager()
{
}

HRESULT CImgui_DataManager::Initialize(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pContext)
{
	m_pDevice = _pDevice;
	m_pContext = _pContext;

	/*세이브를 위한 경로설정과 불러오기를 위한 파일탐색*/
	m_SaveFilePath.m_SavePathBase = "../../Resource/Data/Map/";
	Update_SaveFiles();


	int iSize = m_SaveFilePath.m_SaveFiles.size();

	//이제 저장 누르면 이 경로로 저장될거야!
	m_SaveFilePath.m_CurrentSaveFilePath = m_SaveFilePath.m_SavePathBase + "Terrain" + to_string(iSize) + ".json";


	return S_OK;
}

vector<wstring> CImgui_DataManager::GetImageFiles(const wstring& folderPath)
{
	vector<wstring> result;

	for (auto& entry : fs::recursive_directory_iterator(folderPath))
	{
		//올바르지 않은 파일이라면 continue
		if (!fs::is_regular_file(entry))
			continue;
		                                                                                                                        

		auto path = entry.path();
		auto ext = path.extension().wstring();

		if (ext == L".png" || ext == L".jpg" || ext == L".dds")
			result.push_back(path.wstring());

	}
	return result;

}

void CImgui_DataManager::Active_PlacementMode(PlaceObjectInfo Info)
{

	/// //마우스 placement 활성화시키기 -> Update에서 체크
	Data.m_bPlacementMode = true;

	m_PlaceObjInfo = Info;

	//생성
	if (FAILED(Create_MapObject()))
		return;


}

void CImgui_DataManager::Update_MouseInput()
{
	CheckFalse(Data.m_bPlacementMode);
	CheckNull(m_pPlaceObject);

	//그냥 클릭->뗐으면,

	if (!m_bDrag&& m_pInputManager->IsMouseButtonReleased(0))
	{
		//0,0,0에 위치시키기
		m_pPlaceObject->Get_Transform()->Set_State(STATE::POSITION, _float4(0.f, 0.f, 0.f, 1.f));


		Data.m_bPlacementMode = false;
		m_pPlaceObject = nullptr;
	}


	//드래그중이라면
	if (m_pInputManager->IsMouseDragging())
	{
		//위치따라가기
		_float3 vTargetPos;
		
		if (m_PlaceObjInfo.ObjType != MapObjType::TERRAIN)
		{
			if (CTerrain_Base* pBase = m_pGameInstance->Check_Picking())
			{
				vTargetPos = m_pGameInstance->Get_PickingWorldPos();
				OutputDebugString(L"TerrainPicking\n");
			}

			else if (m_pGrid_Manager->IsCollisionWithGrid())
			{
				vTargetPos = m_pGrid_Manager->Get_GridPickingWorldPos();
				OutputDebugString(L"GridPicking\n");
			}

			else
			{
				vTargetPos = m_pGrid_Manager->Get_MouseWorldPos();
				OutputDebugString(L"WorldPicking\n");
			}
		}

		else
		{
			if (m_pGrid_Manager->IsCollisionWithGrid())
			{
				vTargetPos = m_pGrid_Manager->Get_GridPickingWorldPos();
				OutputDebugString(L"GridPicking\n");
			}

			else
			{
				vTargetPos = m_pGrid_Manager->Get_MouseWorldPos();
				OutputDebugString(L"WorldPicking\n");
			}
		}
		
			

		if (m_pPlaceObject)
		{
			CMapTerrain* pMapTerrain = dynamic_cast<CMapTerrain*>(m_pPlaceObject);
			if (pMapTerrain)
			{
				pMapTerrain->Set_CanPicking(true);
			}
			//위치지정
			m_pPlaceObject->Get_Transform()->Set_State(STATE::POSITION, _float4(vTargetPos.x, vTargetPos.y, vTargetPos.z, 1.f));

		}
		
		m_bDrag = true;
	}
		

	if (m_bDrag && m_pInputManager->IsMouseButtonReleased(0))
	{
		
		Data.m_bPlacementMode = false;
		m_pPlaceObject = nullptr;
		m_bDrag = false;

	}


}



HRESULT CImgui_DataManager::Create_MapObject()
{
	switch (m_PlaceObjInfo.ObjType)
	{
	case MapObjType::TERRAIN:
		return Create_MapTerrain();
		break;


	default:
		return Create_Model();
		break;
	}
}

HRESULT CImgui_DataManager::Create_MapTerrain()
{
	CMapTerrain::MAPTERRAIN_DESC Desc;
	Desc.eRenderGroup = ENUM_TO_UINT(RENDERGROUP::PRIORITY);
	Desc.ObjTag = m_pMapObject_Manager->Generate_UniqueTag(m_PlaceObjInfo.ObjType, m_PlaceObjInfo.TexKey);
	Desc.modelName = m_PlaceObjInfo.TexKey;
	Desc.ObjType = m_PlaceObjInfo.ObjType;

	CModel::MODEL_DSC modelDesc;
	Desc.modelDesc = &modelDesc;

	wstring LayerTag = L"Terrain_Layer";
	wstring ProtoTag = L"MapTerrain";

	CGameObject* pCloneObj = dynamic_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_OBJ_NAME(ProtoTag), &Desc));
	if (pCloneObj)
	{
		CTerrain_Base* pTerrain = dynamic_cast<CTerrain_Base*>(pCloneObj);
		if (pTerrain)
		{
			m_pGameInstance->Register_Terrain(Desc.ObjTag, pTerrain);
			m_pPlaceObject = pCloneObj;

			CMapTerrain* pMapTerrain = dynamic_cast<CMapTerrain*>(pTerrain);
			if (pMapTerrain)
				pMapTerrain->Set_CanPicking(false);		//설치하기전까진 픽킹안됨
			return S_OK;
		}
	}

	return E_FAIL;
}

HRESULT CImgui_DataManager::Create_Model()
{
	CMapModel::MAPMODEL_DESC Desc;
	Desc.eRenderGroup = ENUM_TO_UINT(RENDERGROUP::PRIORITY);
	Desc.ObjTag = m_pMapObject_Manager->Generate_UniqueTag(m_PlaceObjInfo.ObjType, m_PlaceObjInfo.TexKey);
	Desc.modelName = m_PlaceObjInfo.TexKey;
	Desc.ObjType = m_PlaceObjInfo.ObjType;

	CModel::MODEL_DSC modelDesc;
	Desc.modelDesc = &modelDesc;


	////드래그생성이라면,위치가 따로존재함
	//if (bDrag)
	//{
	//	_float3 PickingPos = m_pGrid_Manager->Get_GridPickingWorldPos();
	//	CTransform::TRANSFORM_DESC	 TransDesc;
	//	TransDesc.vLocalPosition = _float4(PickingPos.x, PickingPos.y, PickingPos.z, 1.f);
	//	Desc.TransformDesc = &TransDesc;

	//}

	//타입에 맞게 레이어에 넣어주기.
	wstring LayerTag = L"";
	wstring ProtoTag = L"";			//L"MapOBstalce" , "MapTile", "MapPosition", "MapTrigger"

	switch (m_PlaceObjInfo.ObjType)
	{
	case MapObjType::OBSTACLE:
		LayerTag = L"Obstacle_Layer";
		ProtoTag = L"Model";

		break;
	case MapObjType::TILE:
		LayerTag = L"Tile_Layer";

		break;

	case MapObjType::POSITION:
		LayerTag = L"Position_Layer";
		break;

	case MapObjType::TRIGGER:
		LayerTag = L"Trigger_Layer";
		break;

	default:
		break;
	}

	if (FAILED(m_pMapObject_Manager->Add_MapObject_To_MapLayer(ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_OBJ_NAME(ProtoTag), LayerTag, &Desc)))
		return E_FAIL;

	m_pPlaceObject = m_pMapObject_Manager->Find_MapObject(LayerTag, Desc.ObjTag);


	return S_OK;
}

MapObjType CImgui_DataManager::Get_ObjType_From_Path(const wstring& path)
{
	if (path.find(L"Obstacle"))
		return MapObjType::OBSTACLE;


	if (path.find(L"Tile"))
		return MapObjType::TILE;

	return MapObjType();
}

HRESULT CImgui_DataManager::Update_SaveFiles()
{
	m_SaveFilePath.m_SaveFiles.clear();
	m_SaveFilePath.m_SaveFileNames.clear();
	m_SaveFilePath.m_SaveFileNamesStr.clear();


	for (const auto& entry : fs::recursive_directory_iterator(m_SaveFilePath.m_SavePathBase))
	{
		if (entry.path().extension() == ".json")
		{
			std::string fullPath = entry.path().string();
			std::string fileName = entry.path().stem().string() + ".json"; // 이름만 추출

			
			m_SaveFilePath.m_SaveFiles.push_back(fullPath);    // 경로 저장
			m_SaveFilePath.m_SaveFileNames.push_back(fileName);
		}

	}


	m_SaveFilePath.m_SaveFileNamesStr.reserve(m_SaveFilePath.m_SaveFileNames.size());
	for (auto& name : m_SaveFilePath.m_SaveFileNames)
		m_SaveFilePath.m_SaveFileNamesStr.push_back(name.c_str());

	return S_OK;
}

void CImgui_DataManager::Free()
{
	__super::Free();

	Safe_Release(m_pMapObject_Manager);
	Safe_Release(m_pInputManager);
	Safe_Release(m_pGrid_Manager);

	Safe_Release(m_pGameInstance);


}

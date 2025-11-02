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
}

void CImgui_DataManager::Update_MouseInput()
{
	CheckFalse(Data.m_bPlacementMode);

	//그냥 클릭->뗐으면,
	if (!m_bDrag&& m_pInputManager->IsMouseButtonReleased(0))
	{
		if(FAILED(Create_MapObject(false)))
			return;

		Data.m_bPlacementMode = false;
	}


	//드래그중이라면
	if (m_pInputManager->IsMouseDragging())
		m_bDrag = true;

	if (m_bDrag && m_pInputManager->IsMouseButtonReleased(0))
	{
		if (FAILED(Create_MapObject(true)))
			return;

		Data.m_bPlacementMode = false;
		m_bDrag = false;

	}


}

wstring CImgui_DataManager::Generate_UniqueTag(MapObjType Type, const wstring& baseName)
{
	int iIdx = 0;
	_wstring Result;

	if (Type != MapObjType::TERRAIN)
	{
		CMapLayer* pMapLayer = m_pMapObject_Manager->Get_Layer_By_MapObjType(Type);
		if (pMapLayer)
		{
			while (true)
			{
				Result = baseName + L"_" + to_wstring(iIdx);
				if (pMapLayer->Find_GameObject(Result) == nullptr)
					break;


				++iIdx;
			}
		}

		else
			return baseName + L"_" + to_wstring(iIdx);

	}

	else
	{
		while (true)
		{
			Result = baseName + L"_" + to_wstring(iIdx);
			if (m_pGameInstance->Find_Terrain(Result) == nullptr)
				break;


			++iIdx;
		}
		

		
	}

	return Result;
}


HRESULT CImgui_DataManager::Create_MapObject(bool bDrag)
{
	switch (m_PlaceObjInfo.ObjType)
	{
	case MapObjType::TERRAIN:
		return Create_MapTerrain(bDrag);
		break;


	default:
		return Create_Model(bDrag);
		break;
	}
}

HRESULT CImgui_DataManager::Create_MapTerrain(bool bDrag)
{
	CMapTerrain::MAPTERRAIN_DESC Desc;
	Desc.eRenderGroup = ENUM_TO_UINT(RENDERGROUP::PRIORITY);
	Desc.ObjTag = Generate_UniqueTag(m_PlaceObjInfo.ObjType, m_PlaceObjInfo.TexKey);
	Desc.modelName = m_PlaceObjInfo.TexKey;
	Desc.ObjType = m_PlaceObjInfo.ObjType;

	if (bDrag)
	{
		_float3 PickingPos = m_pGrid_Manager->Get_GridPickingWorldPos();
		CTransform::TRANSFORM_DESC	 TransDesc;
		TransDesc.vLocalPosition = _float4(PickingPos.x, PickingPos.y, PickingPos.z, 1.f);
		Desc.TransformDesc = &TransDesc;

	}


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
			return S_OK;
		}
	}

	return E_FAIL;
}

HRESULT CImgui_DataManager::Create_Model(bool bDrag)
{
	CMapModel::MAPMODEL_DESC Desc;
	Desc.eRenderGroup = ENUM_TO_UINT(RENDERGROUP::PRIORITY);
	Desc.ObjTag = Generate_UniqueTag(m_PlaceObjInfo.ObjType, m_PlaceObjInfo.TexKey);
	Desc.modelName = m_PlaceObjInfo.TexKey;
	Desc.ObjType = m_PlaceObjInfo.ObjType;

	CModel::MODEL_DSC modelDesc;
	Desc.modelDesc = &modelDesc;


	//드래그생성이라면,위치가 따로존재함
	if (bDrag)
	{
		_float3 PickingPos = m_pGrid_Manager->Get_GridPickingWorldPos();
		CTransform::TRANSFORM_DESC	 TransDesc;
		TransDesc.vLocalPosition = _float4(PickingPos.x, PickingPos.y, PickingPos.z, 1.f);
		Desc.TransformDesc = &TransDesc;

	}

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

	return m_pMapObject_Manager->Add_MapObject_To_MapLayer(ENUM_TO_UINT(LEVEL_ID::STATIC), PROTO_OBJ_NAME(ProtoTag), LayerTag, &Desc);


}

MapObjType CImgui_DataManager::Get_ObjType_From_Path(const wstring& path)
{
	if (path.find(L"Obstacle"))
		return MapObjType::OBSTACLE;


	if (path.find(L"Tile"))
		return MapObjType::TILE;

	return MapObjType();
}

void CImgui_DataManager::Free()
{
	__super::Free();

	Safe_Release(m_pMapObject_Manager);
	Safe_Release(m_pInputManager);
	Safe_Release(m_pGrid_Manager);

	Safe_Release(m_pGameInstance);


}

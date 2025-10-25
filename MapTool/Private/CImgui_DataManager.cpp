#include "CImgui_DataManager.h"
#include "MapTool_Defines.h"
#include "CInput_Manager.h"
#include "CMapObject_Manager.h"
#include "CMapQuad.h"
#include "Client_Defines.h"

#include "CMapLayer.h"


USING(MapTool)

IMPLEMENT_SINGLETON(CImgui_DataManager)

CImgui_DataManager::CImgui_DataManager()
	:m_pInputManager(CInput_Manager::GetInstance()),
	m_pMapObject_Manager(CMapObject_Manager::GetInstance())
{
	Safe_AddRef(m_pMapObject_Manager);
	Safe_AddRef(m_pInputManager);
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
	/// <summary>
	/// //마우스 placement 활성화시키기 -> Update에서 체크
	/// </summary>
	/// <param name="Info"></param>
	Data.m_bPlacementMode = true;
	m_PlaceObjInfo = Info;
}

void CImgui_DataManager::Update_MouseInput()
{
	CheckFalse(Data.m_bPlacementMode);

	//그냥 클릭->뗐으면,
	if (!m_bDrag&& m_pInputManager->IsMouseButtonReleased(0))
	{
		CMapQuad::MAPQUAD_DESC Desc;
		Desc.eRenderGroup = ENUM_TO_UINT(RENDERGROUP::ALPHA);
		Desc.ObjTag = Generate_UniqueTag(m_PlaceObjInfo.ObjType,m_PlaceObjInfo.TexKey);
		Desc.TextureKey = m_PlaceObjInfo.TexKey;
		Desc.ObjType = m_PlaceObjInfo.ObjType;

		
		if(m_PlaceObjInfo.ObjType==MapObjType::MODEL)
			if (FAILED(m_pMapObject_Manager->Add_Model_To_MapLayer(&Desc)))
			return;




		Data.m_bPlacementMode = false;
	}


	//드래그중이라면
	if (m_pInputManager->IsMouseDragging())
	{
		m_bDrag = true;

		_float4x4 Proj = CGameInstance::GetInstance()->Get_ProjMatrix(ENUM_TO_UINT(CAMERA_TYPE::FREE));
		_float4x4 View = CGameInstance::GetInstance()->Get_ViewMatrix(ENUM_TO_UINT(CAMERA_TYPE::FREE));


		
	

	}

	if (m_bDrag && m_pInputManager->IsMouseButtonReleased(0))
	{
		int A = 10;
		Data.m_bPlacementMode = false;
		m_bDrag = false;

		//위치받아서 생성
		CMapQuad::MAPQUAD_DESC Desc;
		Desc.eRenderGroup = ENUM_TO_UINT(RENDERGROUP::ALPHA);
		Desc.ObjTag = Generate_UniqueTag(m_PlaceObjInfo.ObjType, m_PlaceObjInfo.TexKey);
		Desc.TextureKey = m_PlaceObjInfo.TexKey;
		Desc.ObjType = m_PlaceObjInfo.ObjType;


		_float3 PickingPos= CGameInstance::GetInstance()->Get_PickingWorldPos();
		CTransform::TRANSFORM_DESC	 TransDesc;
		TransDesc.vLocalPosition = _float4(PickingPos.x, PickingPos.y, PickingPos.z, 1.f);

		Desc.TransformDesc = &TransDesc;



		if (m_PlaceObjInfo.ObjType == MapObjType::MODEL)
			if (FAILED(m_pMapObject_Manager->Add_Model_To_MapLayer(&Desc)))
				return;



	}


}

wstring CImgui_DataManager::Generate_UniqueTag(MapObjType Type, const wstring& baseName)
{
	int iIdx = 0;
	_wstring Result;

	CMapLayer* pMapLayer = Type == MapObjType::MODEL ? m_pMapObject_Manager->Find_MapLayer(L"Model_Layer") : m_pMapObject_Manager->Find_MapLayer(L"Tile_Layer");
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

	return Result;
}

void CImgui_DataManager::Free()
{
	__super::Free();

	Safe_Release(m_pMapObject_Manager);
	Safe_Release(m_pInputManager);

}

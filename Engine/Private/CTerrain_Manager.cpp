#include "CTerrain_Manager.h"
#include "CInput_Manager.h"
#include "MathUtils.h"
#include "CVIBuffer.h"
#include "CMapTerrain.h"
#include "CMapObject_Manager.h"
#include "CModel.h"
#include "CImGui_Manager.h"
#include "CCamera_Base.h"





CTerrain_Manager::CTerrain_Manager(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pContext)
	:m_pDevice{_pDevice},m_pDeviceContext{ _pContext },
	PickingWolrdPos{}, m_EngineDesc{},
	m_pImguiManager{ CImGui_Manager ::GetInstance()},
	m_pGameInstance{CGameInstance::GetInstance()}
{
}

HRESULT CTerrain_Manager::Register_Terrain(const _wstring& Key, CTerrain_Base* pTerrain)
{
	CTerrain_Base* pFindTerrain = Find_Terrain(Key);
	if (pFindTerrain)
	{
		MSG_BOX("Terrain is alreday exist");
		return E_FAIL;
	}

	else
	{
		CMapTerrain* ppTerrain = dynamic_cast<CMapTerrain*>(pTerrain);
		if (ppTerrain)
			ppTerrain->Set_Idx(CAST_INT(m_TerrainMap.size()));

		m_TerrainMap.emplace(Key, pTerrain);
	

	}


	return S_OK;
}

HRESULT CTerrain_Manager::UnRegister_Terrain(const _wstring& Key)
{
	auto iter = m_TerrainMap.find(Key);
	if (iter != m_TerrainMap.end())
	{
		Safe_Release(iter->second);

	}
	return S_OK;
}

void CTerrain_Manager::Update_Priority(_float fTimeDelta)
{
	CheckFalse(m_bUpdate);
	for (auto& pair : m_TerrainMap)
	{
		if (pair.second)
		{
			if(pair.second->Is_Active())
				pair.second->Update_Priority(fTimeDelta);
		}
			
	}
}

void CTerrain_Manager::Update(_float fTimeDelta)
{
	CheckFalse(m_bUpdate);

	for (auto& pair : m_TerrainMap)
	{
		if (pair.second)
		{
			if(pair.second->Is_Active())
				pair.second->Update(fTimeDelta);
		}
			
	}
}

void CTerrain_Manager::Update_Late(_float fTimeDelta)
{
#ifdef  _DEBUG
	ImGuiIO& io = ImGui::GetIO();
	CheckTrue(io.WantCaptureMouse);
#endif //  _DEBUG

	

	CheckFalse(m_bUpdate);

	if (m_pImguiManager->Get_MapToolMode() == MapToolMode::EDIT)
	{
		if (CInput_Manager::GetInstance()->IsMouseButtonPressed(0))
		{
			m_pPickTerrain = Check_Picking();
		}

		
	}
			
	else if (m_pImguiManager->Get_MapToolMode() == MapToolMode::NAVMESH)
		m_pPickTerrain = Check_Picking();

	for (auto& pair : m_TerrainMap)
	{
		if (pair.second)
		{
			if(pair.second->Is_Active())
				pair.second->Update_Late(fTimeDelta);
		}
			
	}

	ProcessDestroy();
}

void CTerrain_Manager::Update_Render(_float fTimeDelta)
{

	CheckFalse(m_bUpdateRender);
	CCamera_Base* pMaincamera = m_pGameInstance->Get_MainCamera();
	CCamera_Base* pMiniMapCamera = m_pGameInstance->Find_Camera(CAMERA_TYPE::MINIMAP);

	CCamera_Base* pRenderCamera = m_pGameInstance->Get_RenderCamera();
		
	for (auto& pair : m_TerrainMap)
	{
		if (pair.second)
		{

			pair.second->Update_Render(fTimeDelta);
			pair.second->Update_Render_MiniMapPriority();


		}
			
	}
}



CTerrain_Base* CTerrain_Manager::Find_Terrain(const _wstring& Key)
{
	auto iter = m_TerrainMap.find(Key);

	if (iter != m_TerrainMap.end())
		return iter->second;

	else
		return nullptr;
	
}

Triangle* CTerrain_Manager::PickTerrain(const _wstring& Key)
{
	

	CTerrain_Base* pTerrain = Find_Terrain(Key);
	CheckNullResult(pTerrain, nullptr);


	//레이를 생성한다.
	//freecam의 view/proj가져오기
	_float4x4 Proj = CGameInstance::GetInstance()->Get_ProjMatrix(ENUM_TO_UINT(CAMERA_TYPE::FREE));
	_float4x4 View = CGameInstance::GetInstance()->Get_ViewMatrix(ENUM_TO_UINT(CAMERA_TYPE::FREE));

	CheckTrueResult(MathUtils::IsZeroMatrix(Proj), nullptr);
	CheckTrueResult(MathUtils::IsZeroMatrix(View), nullptr);


	Ray ray = MathUtils::CreateRayLocal(m_EngineDesc.hWnd, m_pDeviceContext, pTerrain, Proj, View);

	_uint TerrainX = pTerrain->Get_VIBufferCom()->Get_NumVerticesX();
	_uint TerrainZ = pTerrain->Get_VIBufferCom()->Get_NumVerticesZ();

	const vector<_uint>& m_pIndices = pTerrain->Get_VIBufferCom()->Get_Indices();
	const vector<_float3>& m_pPositions = pTerrain->Get_VIBufferCom()->Get_VertexPositions();

	_uint IndicesNum = pTerrain->Get_VIBufferCom()->Get_NumIndices();

	for (_uint i = 0; i < IndicesNum; i += 3)
	{


		_vector p0 = XMLoadFloat3(&m_pPositions[m_pIndices[i]]);
		_vector p1 = XMLoadFloat3(&m_pPositions[m_pIndices[i + 1]]);
		_vector p2 = XMLoadFloat3(&m_pPositions[m_pIndices[i + 2]]);

		float dist = 0.f;
		if (TriangleTests::Intersects(ray.Origin, ray.Dir, p0, p1, p2, dist))
		{
			if (isnan(dist))
				return nullptr;



			PickLocalTriangle.v0 = m_pPositions[m_pIndices[i]];
			PickLocalTriangle.v1 = m_pPositions[m_pIndices[i + 1]];
			PickLocalTriangle.v2 = m_pPositions[m_pIndices[i + 2]];

			return &PickLocalTriangle;

		}



	}



	return nullptr;
}

void CTerrain_Manager::Clear_Terrains()
{
	for (auto& pair : m_TerrainMap)
	{
		if (pair.second)
			Safe_Release(pair.second);
	}


	m_TerrainMap.clear();
}

CTerrain_Base* CTerrain_Manager::Check_Picking()
{
	CheckTrueResult(m_TerrainMap.empty(),nullptr);
	

	
	_float4x4 Proj = CGameInstance::GetInstance()->Get_ProjMatrix(ENUM_TO_UINT(CAMERA_TYPE::FREE));
	_float4x4 View = CGameInstance::GetInstance()->Get_ViewMatrix(ENUM_TO_UINT(CAMERA_TYPE::FREE));

	float Dist = 0.f;
	float MinDist = FLT_MAX;

	CTerrain_Base* pPickObj = nullptr;


	for (auto& pair : m_TerrainMap)
	{
		if (pair.second)
		{
			Ray ray = MathUtils::CreateRayWorld(m_EngineDesc.hWnd, m_pDeviceContext, Proj, View);
			CMapTerrain* pTerrain = dynamic_cast<CMapTerrain*>(pair.second);
			if (pTerrain)
			{
				if (pTerrain->Is_Picked(ray.Origin, ray.Dir, Dist))
				{
					if (Dist < MinDist)
					{
						MinDist = Dist;
						pPickObj = pair.second;
						XMStoreFloat3(&PickingWolrdPos, ray.Origin + ray.Dir * Dist);

						//XMStoreFloat3(&PickingWolrdPos, XMVector3TransformCoord(XMLoadFloat3(&PickingWolrdPos), XMLoadFloat4x4(&pTerrain->Get_Transform()->Get_World(TransformScope::WORLD))));
						
					}
				}
			}
		}
	}


	m_fPickDist = Dist;


	return pPickObj;

}

HRESULT CTerrain_Manager::Save_All_Terrains(const string& path, int iNum)
{
	json jTerrain;
	for (auto& Terrain : m_TerrainMap)
	{
		if (Terrain.second)
		{
			CMapTerrain* pMapTerrain = dynamic_cast<CMapTerrain*>(Terrain.second);
			if (pMapTerrain)
				pMapTerrain->Save_To_Json(jTerrain);
		}
	}



	string FinalPath = "";
	FinalPath = path;

	ofstream	file(FinalPath);
	file << std::setw(4) << jTerrain;

	file.close();


	MSG_BOX("Save Complete");
	return S_OK;

}

HRESULT CTerrain_Manager::Load_Terrains_MapTool(const string& LoadPath)
{
	m_pPickTerrain = nullptr;

	CGameInstance* m_pGameInstance=CGameInstance::GetInstance();
	Clear_Terrains();

	ifstream file(LoadPath);
	json jTerrainData = json::parse(file);
	for (auto& jTerrain : jTerrainData)
	{

		string ModelName = jTerrain["ModelName"];
		int TileID = jTerrain["TileID"];

		json TransformData = jTerrain["Transform"];
		json ChunkInfoData = jTerrain["ChunkInfo"];


		_float3 vPos, vRot, vScale;
		vPos.x = TransformData["Position"][0].get<float>();
		vPos.y = TransformData["Position"][1].get<float>();
		vPos.z = TransformData["Position"][2].get<float>();

		vRot.x = TransformData["Rotation"][0].get<float>();
		vRot.y = TransformData["Rotation"][1].get<float>();
		vRot.z = TransformData["Rotation"][2].get<float>();

		vScale.x = TransformData["Scale"][0].get<float>();
		vScale.y = TransformData["Scale"][1].get<float>();
		vScale.z = TransformData["Scale"][2].get<float>();

		_float2 vIndex = { 0.f,0.f };
		if (!ChunkInfoData.is_null())
		{
			if (!ChunkInfoData["Index"].is_null())
			{
				vIndex.x = ChunkInfoData["Index"][0].get<int>();
				vIndex.y = ChunkInfoData["Index"][1].get<int>();

			}
		}
		
	
		CMapTerrain::MAPTERRAIN_DESC desc;
		desc.eRenderGroup = 0;
		desc.modelName = StringToWString(ModelName);
		desc.ObjTag = CMapObject_Manager::GetInstance()->Generate_UniqueTag(MapObjType::TERRAIN, desc.modelName);
		desc.ObjType = MapObjType::TERRAIN;
		desc.iIdxZX = vIndex;


		CModel::tagModelDesc modelDesc;
		desc.modelDesc = &modelDesc;

		CTransform::TRANSFORM_DESC TransDesc;
		TransDesc.vLocalPosition = _float4(vPos.x, vPos.y, vPos.z, 1.f);
		TransDesc.vLocalRotation = _float4(vRot.x, vRot.y, vRot.z, 1.f);
		TransDesc.vLocalScale = _float4(vScale.x, vScale.y, vScale.z, 1.f);

		desc.TransformDesc = &TransDesc;

		CGameObject* pCloneObj = dynamic_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, 0, PROTO_OBJ_NAME(L"MapTerrain"), &desc));
		if (pCloneObj)
		{
			CTerrain_Base* pTerrain = dynamic_cast<CTerrain_Base*>(pCloneObj);
			if (pTerrain)
			{
				m_pGameInstance->Register_Terrain(desc.ObjTag, pTerrain);
				
			}
		}
		else
			return E_FAIL;


		




	}
	return S_OK;
}

const vector<LOADTERRAINDATA>& CTerrain_Manager::Load_Terrains_Runtime(const string& LoadPath)
{
	CGameInstance* m_pGameInstance = CGameInstance::GetInstance();
	Clear_Terrains();

	ifstream file(LoadPath);
	json jTerrainData = json::parse(file);
	for (auto& jTerrain : jTerrainData)
	{

		string ModelName = jTerrain["ModelName"];
		int TileID = jTerrain["TileID"];

		json TransformData = jTerrain["Transform"];
		json ChunkInfoData = jTerrain["ChunkInfo"];


		_float3 vPos, vRot, vScale;
		vPos.x = TransformData["Position"][0].get<float>();
		vPos.y = TransformData["Position"][1].get<float>();
		vPos.z = TransformData["Position"][2].get<float>();

		vRot.x = TransformData["Rotation"][0].get<float>();
		vRot.y = TransformData["Rotation"][1].get<float>();
		vRot.z = TransformData["Rotation"][2].get<float>();

		vScale.x = TransformData["Scale"][0].get<float>();
		vScale.y = TransformData["Scale"][1].get<float>();
		vScale.z = TransformData["Scale"][2].get<float>();

		_float2 vIndex = { 0.f,0.f };
		if (!ChunkInfoData.is_null())
		{
			if (!ChunkInfoData["Index"].is_null())
			{
				vIndex.x = ChunkInfoData["Index"][0].get<int>();
				vIndex.y = ChunkInfoData["Index"][1].get<int>();

			}
		}

		LOADTERRAINDATA Data;
		Data.ModelName = ModelName;
		Data.vPosition = _float4(vPos.x, vPos.y, vPos.z, 1.f);
		Data.vScale = _float4(vScale.x, vScale.y, vScale.z, 1.f);
		Data.vRotation = _float4(vRot.x, vRot.y, vRot.z, 1.f);
		Data.vIndex = vIndex;

		LoadDatas.push_back(Data);


	}
	return LoadDatas;
}

void CTerrain_Manager::RequestDestroy(CTerrain_Base* pObj)
{
	pObj->Set_Active(false);
	if (pObj == m_pPickTerrain)
		m_pPickTerrain = nullptr;

	m_DestroyQueue.push(pObj);
}

void CTerrain_Manager::ProcessDestroy()
{
	while (!m_DestroyQueue.empty())
	{
		CTerrain_Base* pObj = m_DestroyQueue.front();
		m_DestroyQueue.pop();

		for (auto it = m_TerrainMap.begin(); it != m_TerrainMap.end();)
		{
			if (it->second == pObj)
				it = m_TerrainMap.erase(it);

			else
				++it;

		}



		Safe_Release(pObj);
	}
}



CTerrain_Manager* CTerrain_Manager::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pContext)
{
	CTerrain_Manager* pInstance = new CTerrain_Manager(_pDevice, _pContext);
	pInstance->m_EngineDesc = CGameInstance::GetInstance()->Get_EngineDesc();

	return pInstance;
}

void CTerrain_Manager::Free()
{
	__super::Free();
	
	Clear_Terrains();
}


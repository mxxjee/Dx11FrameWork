#include "CTerrain_Manager.h"
#include "CInput_Manager.h"
#include "MathUtils.h"
#include "CVIBuffer.h"

CTerrain_Manager::CTerrain_Manager(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pContext)
	:m_pDevice{_pDevice},m_pDeviceContext{ _pContext }
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
	for (auto& pair : m_TerrainMap)
	{
		if (pair.second)
			pair.second->Update_Priority(fTimeDelta);
	}
}

void CTerrain_Manager::Update(_float fTimeDelta)
{
	for (auto& pair : m_TerrainMap)
	{
		if (pair.second)
			pair.second->Update(fTimeDelta);
	}
}

void CTerrain_Manager::Update_Late(_float fTimeDelta)
{
	for (auto& pair : m_TerrainMap)
	{
		if (pair.second)
			pair.second->Update_Late(fTimeDelta);
	}
}

void CTerrain_Manager::Update_Render(_float fTimeDelta)
{
	for (auto& pair : m_TerrainMap)
	{
		if (pair.second)
			pair.second->Update_Render(fTimeDelta);
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

	if (CInput_Manager::GetInstance()->IsMouseButtonHeld(0))
	{
		//레이를 생성한다.
		Ray ray = MathUtils::CreateRay(m_EngineDesc.hWnd,m_pDeviceContext,pTerrain);

		_uint TerrainX = pTerrain->Get_VIBufferCom()->Get_NumVerticesX();
		_uint TerrainZ = pTerrain->Get_VIBufferCom()->Get_NumVerticesZ();

		const _uint* m_pIndices = pTerrain->Get_VIBufferCom()->Get_Indices();
		const _float3* m_pPositions = pTerrain->Get_VIBufferCom()->Get_VertexPositions();

		_uint IndicesNum = pTerrain->Get_VIBufferCom()->Get_NumIndices();

		for (_uint i = 0; i < IndicesNum; i+=3)
		{

			
			_vector p0 = XMLoadFloat3(&m_pPositions[m_pIndices[i]]);
			_vector p1 = XMLoadFloat3(&m_pPositions[m_pIndices[i+1]]);
			_vector p2 = XMLoadFloat3(&m_pPositions[m_pIndices[i+2]]);

			float dist = 0.f;
			if (TriangleTests::Intersects(ray.Origin, ray.Dir, p0,p1,p2,dist))
			{
				if (isnan(dist))
					return nullptr;

		

				PickLocalTriangle.v0 = m_pPositions[m_pIndices[i]];
				PickLocalTriangle.v1 = m_pPositions[m_pIndices[i + 1]];
				PickLocalTriangle.v2 = m_pPositions[m_pIndices[i + 2]];

				return &PickLocalTriangle;

			}


		
		}

	}

	return nullptr;
}

CTerrain_Manager* CTerrain_Manager::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pContext)
{
	CTerrain_Manager* pInstance = new CTerrain_Manager(_pDevice, _pContext);
	pInstance->m_EngineDesc = CGameInstance::GetInstance()->Get_EngineDesc();

	return pInstance;
}

void CTerrain_Manager::Free()
{
	for (auto& pair : m_TerrainMap)
	{
		Safe_Release(pair.second);

	}
}


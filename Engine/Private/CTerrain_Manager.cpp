#include "CTerrain_Manager.h"

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

CTerrain_Manager* CTerrain_Manager::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pContext)
{
	CTerrain_Manager* pInstance = new CTerrain_Manager(_pDevice, _pContext);
	return pInstance;
}

void CTerrain_Manager::Free()
{
	for (auto& pair : m_TerrainMap)
	{
		Safe_Release(pair.second);

	}
}


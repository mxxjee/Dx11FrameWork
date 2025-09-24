#include "CObject_Manager.h"
#include "CLayer.h"
#include "CGameInstance.h"
#include "CGameObject.h"

CObject_Manager::CObject_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:m_pDevice{pDevice},
	m_pContext{pContext},
	m_pGameInstance{CGameInstance::GetInstance()}
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CObject_Manager::Initialize(_uint iNumLevels)
{
	m_iNumLevels = iNumLevels;
	m_Layers.resize(iNumLevels);

	return S_OK;
}

void CObject_Manager::Update_Priority(_float fTimeDelta)
{
	for (size_t i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& pair : m_Layers[i])
		{
			pair.second->Update_Priority(fTimeDelta);
		}
	}
}

void CObject_Manager::Update(_float fTimeDelta)
{
	for (size_t i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& pair : m_Layers[i])
		{
			pair.second->Update(fTimeDelta);
		}
	}
}

void CObject_Manager::Update_Late(_float fTimeDelta)
{
	for (size_t i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& pair : m_Layers[i])
		{
			pair.second->Update_Late(fTimeDelta);
		}
	}
}

HRESULT CObject_Manager::Add_GameObject_To_Layer(_uint iProtoLevelIndex, const _wstring& strPrototypeTag, _uint iLayerLevelIndex, const _wstring& strLayerTag, void* pArg)
{
	CGameObject* pCloneObj = dynamic_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, iProtoLevelIndex, strPrototypeTag, pArg));
	CheckNullResult(pCloneObj, E_FAIL);

	CLayer* pLayer = Find_Layer(iLayerLevelIndex, strLayerTag);
	if (!pLayer)
	{
		pLayer = CLayer::Create();
		pLayer->Add_GameObject(pCloneObj);
		m_Layers[iLayerLevelIndex].emplace(strLayerTag, pLayer);

	}

	else
		pLayer->Add_GameObject(pCloneObj);

	return S_OK;
}

const unordered_map<_wstring, CLayer*>& CObject_Manager::Get_Layers(_uint iLevel)
{
	// TODO: 여기에 return 문을 삽입합니다.
	return m_Layers[iLevel];
}

void CObject_Manager::Clear(_uint iLevelIndex)
{
	CheckTrue(iLevelIndex >= m_iNumLevels || m_Layers[iLevelIndex].empty());
	for (auto& pair : m_Layers[iLevelIndex])
	{
		Safe_Release(pair.second);

	}
	
	m_Layers[iLevelIndex].clear();

}

CLayer* CObject_Manager::Find_Layer(_uint iLevelIndex, const _wstring& LayerTag)
{
	auto iter = m_Layers[iLevelIndex].find(LayerTag);
	if (iter == m_Layers[iLevelIndex].end())
		return nullptr;


	return iter->second;
}

CObject_Manager* CObject_Manager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iNumLevels)
{
	CObject_Manager* pInstance = new CObject_Manager(pDevice, pContext);
	if (FAILED(pInstance->Initialize(iNumLevels)))
	{
		MSG_BOX("Failed to Create : CObject_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CObject_Manager::Free()
{
	__super::Free();

	for (size_t i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& pair : m_Layers[i])
		{
			Safe_Release(pair.second);
		}

		m_Layers[i].clear();

	}
	Safe_Release(m_pGameInstance);
}

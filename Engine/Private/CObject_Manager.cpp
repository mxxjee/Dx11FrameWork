#include "CObject_Manager.h"
#include "CLayer.h"
#include "CGameInstance.h"
#include "CGameObject.h"

CObject_Manager::CObject_Manager(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
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

void CObject_Manager::Update_Render(_float fTimeDelta)
{
	for (size_t i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& pair : m_Layers[i])
		{
			pair.second->Update_Render(fTimeDelta);
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
		pCloneObj->Set_SceneID(iLayerLevelIndex);
		m_Layers[iLayerLevelIndex].emplace(strLayerTag, pLayer);

	}

	else
		pLayer->Add_GameObject(pCloneObj);

	return S_OK;
}

HRESULT CObject_Manager::Add_GameObject_To_Layer(_uint iLayerLevelIndex, const _wstring& strLayerTag, CGameObject* pObject)
{

	CLayer* pLayer = Find_Layer(iLayerLevelIndex, strLayerTag);
	if (!pLayer)
	{
		pLayer = CLayer::Create();
		pLayer->Add_GameObject(pObject);
		pObject->Set_SceneID(iLayerLevelIndex);
		m_Layers[iLayerLevelIndex].emplace(strLayerTag, pLayer);

	}

	else
		pLayer->Add_GameObject(pObject);

	return S_OK;
}

HRESULT CObject_Manager::Make_New_Layer(_uint iLayerLevelIndex, const _wstring& strLayerTag)
{
	//이미 있는지확인하자
	CLayer* pLayer = Find_Layer(iLayerLevelIndex, strLayerTag);
	if (!pLayer)
	{
		pLayer = CLayer::Create();
		m_Layers[iLayerLevelIndex].emplace(strLayerTag, pLayer);
	}

	return S_OK;
}

const unordered_map<_wstring, CLayer*>& CObject_Manager::Get_Layers(_uint iLevel)
{
	// TODO: 여기에 return 문을 삽입합니다.
	return m_Layers[iLevel];
}



CGameObject* CObject_Manager::Find_GameObject(_uint iLevelIndex, const _wstring& LayerTag, const _wstring& Tag)
{
	CheckTrueResult(iLevelIndex >= m_iNumLevels || m_Layers[iLevelIndex].empty(),nullptr);
	CLayer* pLayer = Find_Layer(iLevelIndex, LayerTag);
	if (pLayer)
	{
		return pLayer->Find_GameObject(Tag);
	}
	
	return nullptr;
}

void CObject_Manager::Clear(_uint iLevelIndex)
{
	CheckTrue(iLevelIndex >= m_iNumLevels || iLevelIndex == 0 || m_Layers[iLevelIndex].empty());
	for (auto& pair : m_Layers[iLevelIndex])
	{
		Safe_Release(pair.second);

	}
	
	m_Layers[iLevelIndex].clear();

}

void CObject_Manager::Update_Priority_Static(_float fTimeDelta)
{
	for (auto& pair : m_Layers[0])
	{
		pair.second->Update_Priority(fTimeDelta);
	}
}

void CObject_Manager::Update_Static(_float fTimeDelta)
{
	for (auto& pair : m_Layers[0])
	{
		pair.second->Update(fTimeDelta);
	}
}

void CObject_Manager::Update_Late_Static(_float fTimeDelta)
{
	for (auto& pair : m_Layers[0])
	{
		pair.second->Update_Late(fTimeDelta);
	}
}

void CObject_Manager::Update_Render_Static(_float fTimeDelta)
{
	for (auto& pair : m_Layers[0])
	{
		pair.second->Update_Render(fTimeDelta);
	}
}

CLayer* CObject_Manager::Find_Layer(_uint iLevelIndex, const _wstring& LayerTag)
{
	auto iter = m_Layers[iLevelIndex].find(LayerTag);
	if (iter == m_Layers[iLevelIndex].end())
		return nullptr;


	return iter->second;
}

CObject_Manager* CObject_Manager::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, _uint iNumLevels)
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

	m_Layers.clear();

	m_pDevice = nullptr;
	m_pContext = nullptr;


	Safe_Release(m_pGameInstance);
}

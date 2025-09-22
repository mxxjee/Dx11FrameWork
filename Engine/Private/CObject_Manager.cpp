#include "CObject_Manager.h"
#include "CLayer.h"
#include "CGameInstance.h"
#include "CGameObject.h"

CObject_Manager::CObject_Manager(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:m_pDevice{pDevice},m_pContext{pContext}, m_pGameInstance{CGameInstance::GetInstance()}
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CObject_Manager::Initialize(_uint iNumLevels)
{
	m_iNumLevels = iNumLevels;
	m_Layers.reserve(iNumLevels);
	return S_OK;
}

void CObject_Manager::Update_Priority(_float fTimeDelta)
{
	for (size_t i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& pair : m_Layers[i])
			pair.second->Update_Priority(fTimeDelta);
	}
}

void CObject_Manager::Update(_float fTimeDelta)
{
	for (size_t i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& pair : m_Layers[i])
			pair.second->Update(fTimeDelta);
	}
}

void CObject_Manager::Update_Late(_float fTimeDelta)
{
	for (size_t i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& pair : m_Layers[i])
			pair.second->Update_Late(fTimeDelta);
	}
}

HRESULT CObject_Manager::Add_GameObject_To_Layer(_uint iProtoLevelIndex, const _wstring& strPrototypeTag, _uint iLayerLevelIndex, const _wstring& strLayerTag, void* pArg)
{
	CGameObject* pGameObject = dynamic_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, iProtoLevelIndex, strPrototypeTag, pArg));
	if (nullptr == pGameObject)
		return E_FAIL;


	CLayer* pLayer = Find_Layer(iProtoLevelIndex, strLayerTag);
	if (pLayer)
		pLayer->Add_GameObject(pGameObject);
	

	else
	{
		/*레이어가 없다면 새로만들어서 넣어주기.*/
		CLayer* pNewLayer = CLayer::Create();
		pLayer->Add_GameObject(pGameObject);
		m_Layers[iLayerLevelIndex].emplace(strLayerTag, pLayer);


	}


	return S_OK;
}

CLayer* CObject_Manager::Find_Layer(_uint iNumLevel, const _wstring& strLayerTag)
{
	auto iter = m_Layers[iNumLevel].find(strLayerTag);
	if (iter == m_Layers[iNumLevel].end())
		return nullptr;


	else
		return iter->second;


}

CObject_Manager* CObject_Manager::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, _uint iNumLevels)
{

	CObject_Manager* pInstance = new CObject_Manager(pDevice, pContext);

	if (FAILED(pInstance->Initialize(iNumLevels)))
	{
		MSG_BOX("Failed to Created : CObject_Manager");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CObject_Manager::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
}

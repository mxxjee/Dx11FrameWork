#include "CUI_Manager.h"
#include "CUI.h"

CUI_Manager::CUI_Manager()
{
}

HRESULT CUI_Manager::Register_UIGroup(const _wstring& Key, const UIGroup& Group)
{
	UIGroup* pGroup = Get_UIGroup(Key);
	if (pGroup)
		return E_FAIL;


	else
	{
		m_UIMap.emplace(Key, Group);
		for (auto& i : Group.Objects)
			Safe_AddRef(i);

	}


	return S_OK;
}

HRESULT CUI_Manager::AddUIToGroup(const _wstring& Key, CGameObject* pGameObject)
{
	UIGroup* pGroup = Get_UIGroup(Key);
	if (!pGroup)
		return E_FAIL;


	else
	{
		pGroup->Objects.push_back(pGameObject);
		Safe_AddRef(pGameObject);
	}


	return S_OK;
}

HRESULT CUI_Manager::RegisterEvent(const _wstring& Key, function<void(void*)> _function)
{
	//이벤트를 등록한다. 중복검사
	function<void(void*)>		pEvent = Get_EventFunction(Key);
	if (pEvent)
		return E_FAIL;

	m_EventMap.emplace(Key, _function);
	return S_OK;
}

HRESULT CUI_Manager::BroadCastEvent(const _wstring& Key, void* pData)
{
	function<void(void*)>		pEvent = Get_EventFunction(Key);
	if (!pEvent)
		return E_FAIL;

	pEvent(pData);
	return S_OK;
}

HRESULT CUI_Manager::SetActiveGroup(const _wstring& Key, bool bActive)
{
	UIGroup* pGroup = Get_UIGroup(Key);
	if (!pGroup)
		return E_FAIL;

	for (auto& i : pGroup->Objects)
	{
		i->Set_Active(true);
	}

	return S_OK;
}

UIGroup* CUI_Manager::Get_UIGroup(const _wstring Key)
{
	auto iter = m_UIMap.find(Key);
	if (iter != m_UIMap.end())
		return &(iter->second);



	return nullptr;
}

function<void(void*)> CUI_Manager::Get_EventFunction(const _wstring& Key)
{
	auto iter = m_EventMap.find(Key);
	if (iter != m_EventMap.end())
		return iter->second;

	else
		return nullptr;
}

CUI_Manager* CUI_Manager::Create()
{
	return new CUI_Manager();
}

void CUI_Manager::Free()
{
	for (auto& pair : m_UIMap)
	{
		for (auto& i :pair.second.Objects)
			Safe_Release(i);
			
	}
}

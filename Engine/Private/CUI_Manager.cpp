#include "CUI_Manager.h"
#include "CUI.h"

CUI_Manager::CUI_Manager()
{
}

HRESULT CUI_Manager::Register_UIGroup(const UIGroup& Group, const _wstring& Key)
{
	_wstring FindKey = Key;
	if (FindKey == L"")
		FindKey = Group.Key;


	UIGroup* pGroup = Get_UIGroup(FindKey);
	if (pGroup)
		return E_FAIL;


	else
	{
		size_t Hash = hash<wstring>()(FindKey);
		m_UIMap.emplace(Hash, Group);
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

	size_t Hash = hash<wstring>()(Key);
	m_EventMap.emplace(Hash, _function);
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
		CUI* pUI = dynamic_cast<CUI*>(i);

		//활성/비활성화에 따른 이벤트호출
		if (pUI)
			pUI->OnActivated(bActive);
	}

	return S_OK;
}

UIGroup* CUI_Manager::Get_UIGroup(const _wstring Key)
{
	size_t Hash = hash<wstring>()(Key);

	auto iter = m_UIMap.find(Hash);
	if (iter != m_UIMap.end())
		return &(iter->second);



	return nullptr;
}

function<void(void*)> CUI_Manager::Get_EventFunction(const _wstring& Key)
{
	size_t Hash = hash<wstring>()(Key);

	auto iter = m_EventMap.find(Hash);
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

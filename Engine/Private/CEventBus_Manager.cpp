#include "CEventBus_Manager.h"

CEventBus_Manager::CEventBus_Manager()
{
}

HRESULT CEventBus_Manager::Initialize()
{
	return S_OK;
}

void CEventBus_Manager::Emit(const GameEvent& Event)
{
	m_GameEventQue.push_back(Event);
}

void CEventBus_Manager::RegisterListners(const string& CBName, EventCallBack Callback)
{
	m_EventCB[CBName].push_back(Callback);
}

void CEventBus_Manager::DisPatch(_float fTimeDelta)
{
	for (auto& event : m_GameEventQue)
	{
		auto Target = m_EventCB.find(event.Name);

		if (Target == m_EventCB.end())		//이 이벤트와 맞는 이름을 가진 콜백함수가없네유..
			continue;

		for (auto& callback : Target->second)
			callback(event);


	}

	m_GameEventQue.clear();
}

CEventBus_Manager* CEventBus_Manager::Create()
{
	CEventBus_Manager* pInstance = new CEventBus_Manager();
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Create : CEventBus_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEventBus_Manager::Free()
{
	__super::Free();
	m_GameEventQue.clear();

	for (auto& pair : m_EventCB)
	{
		pair.second.clear();
	}


	m_EventCB.clear();


}

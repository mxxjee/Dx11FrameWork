#include "CTimer_Manager.h"
#include "CTimer.h"


CTimer_Manager::CTimer_Manager()
{
}

_float CTimer_Manager::Get_TimeDelta(const _tchar* pTimerTag)
{
	CTimer* pTimer = Find_Timer(pTimerTag);
	if (nullptr == pTimer)
		return 0.f;

	return pTimer->Get_TimeDelta();
}

void CTimer_Manager::Compute_TimeDelta(const _tchar* pTimerTag)
{
	CTimer* pTimer = Find_Timer(pTimerTag);
	if (nullptr == pTimer)
		return;

	pTimer->Update_Timer();
}

HRESULT CTimer_Manager::Add_Timer(const _tchar* pTimerTag)
{
	CTimer* pTimer = Find_Timer(pTimerTag);

	if (nullptr != pTimer)
		return E_FAIL;

	pTimer = CTimer::Create();
	if (nullptr == pTimer)
		return E_FAIL;

	m_mapTimer.emplace( pTimerTag, pTimer );

	return S_OK;
}

CTimer_Manager* CTimer_Manager::Create()
{
	return new CTimer_Manager;
}

_float CTimer_Manager::Get_FPS(const _tchar* pTimerTag)
{
	CTimer* TargetTimer = Find_Timer(pTimerTag);
	if (TargetTimer)
		return TargetTimer->GetFPS();

	return _float();
}

CTimer* CTimer_Manager::Find_Timer(const _tchar* pTimerTag)
{
	auto		iter = m_mapTimer.find(pTimerTag);


	if (iter == m_mapTimer.end())
		return nullptr;

	return iter->second;
}

void CTimer_Manager::Free()
{
	for (auto& pair : m_mapTimer)
		Safe_Release(pair.second);


	m_mapTimer.clear();
}

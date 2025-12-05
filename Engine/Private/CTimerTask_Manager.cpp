#include "CTimerTask_Manager.h"

_uint CTimerTask_Manager::m_LastTraskId = -1;

CTimerTask_Manager::CTimerTask_Manager()
{
}

CTimerTask_Manager* CTimerTask_Manager::Create()
{
	return new CTimerTask_Manager;
}

void CTimerTask_Manager::Free()
{
	//
}

void CTimerTask_Manager::Update(_float fTimeDelta)
{
	for (auto& Task : m_Tasks)
	{
		Task.m_fTime -= fTimeDelta;
		
		//시간다됐으면..
		if (Task.m_fTime <= 0.f)
		{
			if (Task.m_Callback)
				Task.m_Callback();

			
			if (Task.m_bRepeat)
				Task.m_fTime += Task.m_finterval;

			else
				CancelTask(Task.m_id);

		}
	}
}

HRESULT CTimerTask_Manager::Invoke(float _fTime, float _finterval, bool _bRepeat, bool _bCancelled, std::function<void()> cb, CGameObject* pOwner)
{
	CheckNullResult(pOwner,E_FAIL);
	return AddTask(_fTime, _finterval, false, _bCancelled, cb, pOwner);
}

HRESULT CTimerTask_Manager::Repeat(float _fTime, float _finterval, bool _bRepeat, bool _bCancelled, std::function<void()> cb, CGameObject* pOwner)
{
	CheckNullResult(pOwner, E_FAIL);
	return AddTask(_fTime, _finterval, true, _bCancelled, cb, pOwner);
}

HRESULT CTimerTask_Manager::CancelTaskOf(CGameObject* pOwner)
{
	auto iter = m_OwnerTasks.find(pOwner);
	if (iter == m_OwnerTasks.end())
		return S_OK;		//이 오너가 등록한 태스크가 없다면 그냥 E_FAIL리턴


	for (auto& id : iter->second)
	{
		CancelTask(id);
	}

	m_OwnerTasks.erase(pOwner);

	return S_OK;
}

HRESULT CTimerTask_Manager::AddTask(float _fTime, float _finterval, bool _bRepeat, bool _bCancelled, std::function<void()> cb, CGameObject* pOwner)
{
	Task task;
	task.m_fTime = _fTime;
	task.m_finterval = _finterval;
	task.m_bRepeat = _bRepeat;
	task.cancelled = _bCancelled;
	task.m_Callback = cb;
	task.m_id = ++m_LastTraskId;
	task.pOwner = pOwner;

	//이댸 addref해야되나


	m_Tasks.push_back(task);
	int iLastIndex = m_Tasks.size();

	m_TaskIndex.emplace(task.m_id, iLastIndex - 1);


	return S_OK;
}

HRESULT CTimerTask_Manager::CancelTask(_uint id)
{
	//Swap and erase
	auto it = m_TaskIndex.find(id);
	if (it == m_TaskIndex.end())
		return E_FAIL;

	size_t index = it->second;
	size_t lastIndex = m_Tasks.size() - 1;	


	//마지막 요소와 swap
	swap(m_Tasks[index], m_Tasks[lastIndex]);
	
	//swap한 요소의 id를 찾아 index갱신
	m_TaskIndex[m_Tasks[index].m_id] = index;

	m_Tasks.pop_back();
	m_TaskIndex.erase(id);

	
	return S_OK;
}

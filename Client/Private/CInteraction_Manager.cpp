#include "CInteraction_Manager.h"
#include "CIInteractable.h"
#include "CInput_Manager.h"

USING(Client)
IMPLEMENT_SINGLETON(CInteraction_Manager)
void CInteraction_Manager::RegisterInteractable(string SceneName, CIInteractable* pObj)
{

	vector<CIInteractable*>*		Interactables=Find_InteractionObjects_By_SceneName(SceneName);
	if (Interactables == nullptr)
	{
		size_t Key = hash<string>()(SceneName);

		vector<CIInteractable*>		Objs;
		Objs.push_back(pObj);

		m_sceneInteractbles.emplace(Key, Objs);

	}

	else
	{
		CIInteractable* pTarget = Find_Object(SceneName,pObj);
		if (!pTarget)
			m_InteractableObjects.push_back(pObj);
	}
	

	
}

void CInteraction_Manager::UnRegisterInteractable(string SceneName, const CIInteractable* pObj)
{
	vector<CIInteractable*>* Interactables = Find_InteractionObjects_By_SceneName(SceneName);
	if (Interactables)
	{
		vector<CIInteractable*>::iterator pFindObj = find(Interactables->begin(), Interactables->end(), pObj);
		if (pFindObj != Interactables->end())
			Interactables->erase(pFindObj);
	}

	else
		return;
}

void CInteraction_Manager::Update(_float fTimeDelta)
{
	//최적의 InteratableOBject를 찾아서 저장
	CIInteractable* pBest = nullptr;

	//전체 리스트를 돌면서 상호작용가능한 조건을 가지는 애들을 간추리기
	for (auto pInteratable : (*MainInteractbles))
	{
		if (!pInteratable)
			continue;

		bool inRange = pInteratable->IsInteratable();//일단 인터렉트 가능한 상태인가요?

		//이전프레임에없엇는데 현재들어왔따면
		if (!pInteratable->m_bPrevRange && inRange)
			pInteratable->Enter_InteractRange();

		else if (pInteratable->m_bPrevRange && inRange)
			pInteratable->Stay_InteractRange(fTimeDelta);

		else if (pInteratable->m_bPrevRange && !inRange)
			pInteratable->Exit_InteractRange();


		pInteratable->m_bPrevRange = inRange;


		if (inRange)
			if (!pBest || pInteratable->Get_Interaction_Priority() > pBest->Get_Interaction_Priority())
				pBest = pInteratable;
	}

	m_pCurrentTarget = pBest;


	if (m_pCurrentTarget && m_pCurrentTarget->m_bPrevInteracting)
		m_pCurrentTarget->Stay_Interaction(fTimeDelta);


	for (auto obj : (*MainInteractbles))
	{
		if (obj->m_bPrevInteracting && obj != m_pCurrentTarget)
		{
			obj->Exit_Interaction();
			obj->m_bPrevInteracting = false;
		}
	}

}

bool CInteraction_Manager::OnInteractKeyPresed()
{
	if (CInput_Manager::GetInstance()->IsKeyPressed(KeyCode::A))
	{
		if (!m_pCurrentTarget)
			return false;

		if (!m_pCurrentTarget->m_bPrevInteracting)
		{
			m_pCurrentTarget->Enter_Interaction();
			m_pCurrentTarget->m_bPrevInteracting = true;
			return true;
		}


	}
	
	return false;

}

void CInteraction_Manager::Clear()
{
	m_pCurrentTarget = nullptr;

	for (auto& pair : m_sceneInteractbles)
	{
		pair.second.clear();
	}

}

bool CInteraction_Manager::Check_InteractiveType(InteractionType eType)
{
	CheckNullResult(m_pCurrentTarget, false);

	return m_pCurrentTarget->Get_Interaction_Priority() == (int)eType;
}

CIInteractable* CInteraction_Manager::Find_Object(string SceneName, const CIInteractable* pObj)
{
	size_t Key = hash<string>()(SceneName);
	
	vector<CIInteractable*>* pFindVector = Find_InteractionObjects_By_SceneName(SceneName);
	if (!pFindVector)
		nullptr;



	vector<CIInteractable*>::iterator pFindObj = find(pFindVector->begin(), pFindVector->end(), pObj);


	if (pFindObj != pFindVector->end())
		return (*pFindObj);

	else
		return nullptr;


	return nullptr;
}

vector<CIInteractable*>* CInteraction_Manager::Find_InteractionObjects_By_SceneName(string SceneName)
{
	size_t SceneID = hash<string>()(SceneName);
	auto iter = m_sceneInteractbles.find(SceneID);

	if (iter == m_sceneInteractbles.end())
		return nullptr;


	return &iter->second;
}

void CInteraction_Manager::Free()
{
	
	
	m_pCurrentTarget = nullptr;
	m_InteractableObjects.clear();

	__super::Free();
}

HRESULT CInteraction_Manager::Set_MainInteratables(string SceneName)
{
	vector<CIInteractable*>* Interactables = Find_InteractionObjects_By_SceneName(SceneName);
	if (Interactables)
		MainInteractbles = Interactables;


	else
	{
		MainInteractbles = nullptr;
		return E_FAIL;

	}


	return S_OK;
}



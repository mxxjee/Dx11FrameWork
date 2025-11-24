#include "CInteraction_Manager.h"
#include "CIInteractable.h"
#include "CInput_Manager.h"

USING(Client)
IMPLEMENT_SINGLETON(CInteraction_Manager)
void CInteraction_Manager::RegisterInteractable(CIInteractable* pObj)
{
	CIInteractable* pTarget = Find_Object(pObj);
	if (!pTarget)
		m_InteractableObjects.push_back(pObj);
}

void CInteraction_Manager::UnRegisterInteractable(const CIInteractable* pObj)
{
	auto pFindObj = find(m_InteractableObjects.begin(), m_InteractableObjects.end(), pObj);

	if (pFindObj != m_InteractableObjects.end())
		m_InteractableObjects.erase(pFindObj);
	 
	else
		return;
}

void CInteraction_Manager::Update(_float fTimeDelta)
{
	//최적의 InteratableOBject를 찾아서 저장
	CIInteractable* pBest = nullptr;

	//전체 리스트를 돌면서 상호작용가능한 조건을 가지는 애들을 간추리기
	for (auto pInteratable : m_InteractableObjects)
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


	for (auto obj : m_InteractableObjects)
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
	m_InteractableObjects.clear();
}

bool CInteraction_Manager::Check_InteractiveType(InteractionType eType)
{
	CheckNullResult(m_pCurrentTarget, false);

	return m_pCurrentTarget->Get_Interaction_Priority() == (int)eType;
}

CIInteractable* CInteraction_Manager::Find_Object(const CIInteractable* pObj)
{
	auto pFindObj = find(m_InteractableObjects.begin(), m_InteractableObjects.end(), pObj);


	if (pFindObj != m_InteractableObjects.end())
		return (*pFindObj);

	return nullptr;
}

void CInteraction_Manager::Free()
{
	__super::Free();

	m_InteractableObjects.clear();
}



#include "CInteraction_Manager.h"
#include "CIInteractable.h"
#include "CInput_Manager.h"
#include "Client_Defines.h"
#include "CInteractionObject.h"
#include "CPlayer.h"
#include "CGameInstance.h"
#include "CLayer.h"
#include "CGameManager.h"
#include <algorithm>

USING(Client)
IMPLEMENT_SINGLETON(CInteraction_Manager)
void CInteraction_Manager::RegisterInteractable(CIInteractable* pObj)
{
	CheckNull(pObj);

	CIInteractable* pTarget = Find_Object(pObj);

	if (!pTarget)
	{
		m_InteractableObjects.push_back(pObj);
	}
}



void CInteraction_Manager::UnRegisterInteractable(const CIInteractable* pObj)
{
	PurgeInteractable(const_cast<CIInteractable*>(pObj));
}

void CInteraction_Manager::RequestAddCandidate(CIInteractable* pObj)
{
	RequestCandidateState(pObj, true);
}

void CInteraction_Manager::RequestRemoveCandidate(CIInteractable* pObj)
{
	RequestCandidateState(pObj, false);
}

void CInteraction_Manager::RequestCandidateState(CIInteractable* pObj, bool bAdd)
{
	CheckNull(pObj);

	for (CANDIDATE_REQUEST& Request : m_PendingCandidateRequests)
	{
		if (Request.pObject == pObj)
		{
			Request.bAdd = bAdd;
			return;
		}
	}

	m_PendingCandidateRequests.push_back({ pObj, bAdd });
}

void CInteraction_Manager::ApplyPendingCandidates()
{
	std::vector<CANDIDATE_REQUEST> Requests;
	Requests.swap(m_PendingCandidateRequests);

	for (const CANDIDATE_REQUEST& Request : Requests)
	{
		CIInteractable* pObject = Request.pObject;
		if (!pObject)
			continue;

		auto CandidateIter = std::find(m_Candidates.begin(), m_Candidates.end(), pObject);
		if (Request.bAdd)
		{
			if (Find_Object(pObject) && CandidateIter == m_Candidates.end())
				m_Candidates.push_back(pObject);
		}
		else if (CandidateIter != m_Candidates.end())
		{
			m_Candidates.erase(CandidateIter);
			if (pObject->m_bPrevRange)
			{
				pObject->Exit_InteractRange();
				pObject->m_bPrevRange = false;
			}
		}
	}
}

void CInteraction_Manager::PurgeInteractable(CIInteractable* pObj)
{
	CheckNull(pObj);

	if (pObj->m_bPrevInteracting)
	{
		pObj->Exit_Interaction();
		pObj->m_bPrevInteracting = false;
	}

	if (pObj->m_bPrevRange)
	{
		pObj->Exit_InteractRange();
		pObj->m_bPrevRange = false;
	}

	m_InteractableObjects.remove(pObj);
	m_Candidates.erase(std::remove(m_Candidates.begin(), m_Candidates.end(), pObj), m_Candidates.end());
	m_PendingCandidateRequests.erase(
		std::remove_if(
			m_PendingCandidateRequests.begin(),
			m_PendingCandidateRequests.end(),
			[pObj](const CANDIDATE_REQUEST& Request) { return Request.pObject == pObj; }),
		m_PendingCandidateRequests.end());

	if (m_pCurrentTarget == pObj)
		m_pCurrentTarget = nullptr;
	if (m_pPreTarget == pObj)
		m_pPreTarget = nullptr;
}

void CInteraction_Manager::Update(_float fTimeDelta)
{
	ApplyPendingCandidates();

	if (!m_pMainPlayer)
		return;

	if (m_InteractableObjects.empty())
		return;
	
	if (CGameManager::GetInstance()->Get_EndingStep() == EndingStep::EPILOGUE)
		return;

	// 플레이어가 TriggerBox를 벗어나 후보에서 제거되더라도,
	// 상호작용 생명주기가 명시적으로 끝날 때까지 현재 대상을 유지한다.
	if (m_pCurrentTarget && m_pCurrentTarget->m_bPrevInteracting)
	{
		m_pCurrentTarget->Stay_Interaction(fTimeDelta);
		return;
	}

	CIInteractable* pBest = nullptr;
	CIInteractable* pPreviousTarget = m_pCurrentTarget;

	// 플레이어와 겹친 후보만 프레임별 대상 탐색에 참여한다.
	// 엄격한 '>' 비교를 사용해 우선순위가 같으면 먼저 진입한 순서를 유지한다.
	for (CIInteractable* pInteratable : m_Candidates)
	{
		if (!pInteratable)
			continue;

		bool inRange = pInteratable->IsInteratable();

		// 광역 충돌 후보 내부에서도 기존 범위 생명주기 콜백을 유지한다.
		if (!pInteratable->m_bPrevRange && inRange)
		{
			pInteratable->Enter_InteractRange();
		}

		else if (pInteratable->m_bPrevRange && inRange)
			pInteratable->Stay_InteractRange(fTimeDelta);

		else if (pInteratable->m_bPrevRange && !inRange)
		{
			pInteratable->Exit_InteractRange();
		}

		pInteratable->m_bPrevRange = inRange;

		if (inRange)
		{
			if (!pBest || pInteratable->Get_Interaction_Priority() > pBest->Get_Interaction_Priority())
				pBest = pInteratable;
		}
	}

	m_pCurrentTarget = pBest;
	if (pPreviousTarget != m_pCurrentTarget)
		m_pPreTarget = pPreviousTarget;

	// 위의 대상 고정으로 전체 등록 목록을 두 번째 순회할 필요가 없다.
	// 생명주기 방어를 위해 이전 대상 정리만 유지한다.
	if (pPreviousTarget && pPreviousTarget != m_pCurrentTarget && pPreviousTarget->m_bPrevInteracting)
	{
		pPreviousTarget->Exit_Interaction();
		pPreviousTarget->m_bPrevInteracting = false;
	}
}

bool CInteraction_Manager::OnInteractKeyPresed()
{
	CheckNullResult(m_pMainPlayer, false);
	CheckTrueResult(m_pMainPlayer->Get_ActionControl()->m_bItemGet, false);

	if (CInput_Manager::GetInstance()->IsKeyPressed(KeyCode::A))
	{
		if (!m_pCurrentTarget)
			return false;

		if (CGameManager::GetInstance()->Get_EndingStep() != EndingStep::EPILOGUE)
		{
			if (!m_pCurrentTarget->m_bPrevInteracting)
			{
				m_pCurrentTarget->Enter_Interaction();
				m_pCurrentTarget->m_bPrevInteracting = true;
				return true;
			}

			else
			{
				if (m_pCurrentTarget->Get_Interaction_Priority() != ENUM_TO_UINT(InteractionType::NPC))
				{
					m_pCurrentTarget->Exit_Interaction();
					m_pCurrentTarget->m_bPrevInteracting = false;
					m_pCurrentTarget->m_bPrevRange = false;
					return true;
				}

			}
		}
		m_pCurrentTarget->Pressed_InteractionKey();

	}
	
	return false;

}

void CInteraction_Manager::Clear()
{
	// 씬 또는 객체 생명주기 정리에서는 전체 등록 목록을 순회할 수 있지만, 프레임별 대상 탐색에는 사용하지 않는다.
	for (auto& pObj : m_InteractableObjects)
	{
		if (pObj)
		{
			// 활성화된 상호작용 및 범위 상태에 대응하는 종료 콜백을 호출한다.
			if (pObj->m_bPrevInteracting)
			{
				pObj->Exit_Interaction();
				pObj->m_bPrevInteracting = false;
			}

			if (pObj->m_bPrevRange)
			{
				pObj->Exit_InteractRange();
				pObj->m_bPrevRange = false;
			}
		}
	}
	m_InteractableObjects.clear();
	m_Candidates.clear();
	m_PendingCandidateRequests.clear();
	m_pCurrentTarget = nullptr;
	m_pPreTarget = nullptr;
}

bool CInteraction_Manager::Check_InteractiveType(InteractionType eType)
{
	CheckNullResult(m_pCurrentTarget, false);

	return m_pCurrentTarget->Get_Interaction_Priority() == (int)eType;
}

void CInteraction_Manager::Add_Interaction(CIInteractable* pObj)
{
	RegisterInteractable(pObj);
}

void CInteraction_Manager::Change_Scene(_uint iLevelID)
{
	Clear();

	CLayer* pInteractionLayer = m_pGameInstance->Find_Layer(iLevelID, L"Interaction_Layer");
	if (pInteractionLayer)
	{
		for (auto& pObj : pInteractionLayer->Get_ObjList())
		{
			CIInteractable* pInteractable = dynamic_cast<CIInteractable*>(pObj);
			if (pInteractable)
				CInteraction_Manager::GetInstance()->RegisterInteractable(pInteractable);
		}
	}
	

	CLayer* pNPCLayer = m_pGameInstance->Find_Layer(iLevelID, L"NPC_Layer");
	if (pNPCLayer)
	{
		for (auto& pObj : pNPCLayer->Get_ObjList())
		{
			CIInteractable* pInteractable = dynamic_cast<CIInteractable*>(pObj);
			if (pInteractable)
				CInteraction_Manager::GetInstance()->RegisterInteractable(pInteractable);
		}
	}
}

HRESULT CInteraction_Manager::Initialize()
{
	m_pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(m_pGameInstance);
	m_Candidates.reserve(8);
	m_PendingCandidateRequests.reserve(8);

	return S_OK;
}



CIInteractable* CInteraction_Manager::Find_Object(const CIInteractable* pObj)
{
	
	
	list<CIInteractable*>::iterator pFindObj = find(m_InteractableObjects.begin(), m_InteractableObjects.end(), pObj);


	if (pFindObj != m_InteractableObjects.end())
		return (*pFindObj);

	else
		return nullptr;


	return nullptr;
}

void CInteraction_Manager::Set_MainPlayer(CGameObject* pObj)
{
	m_pMainPlayer=dynamic_cast<CPlayer*>(CGameInstance::GetInstance()->Find_GameObject(ENUM_TO_UINT(LEVEL_ID::STATIC), L"Player_Layer", L"Player"));
	Safe_AddRef(m_pMainPlayer);

}


void CInteraction_Manager::Set_CurrentTarget(CIInteractable* pObj)
{
	m_pCurrentTarget = pObj; 
	m_pCurrentTarget->Enter_Interaction();
	m_pCurrentTarget->m_bPrevInteracting = true;
	
}

void CInteraction_Manager::Free()
{
	m_pCurrentTarget = nullptr;
	m_pPreTarget = nullptr;
	m_InteractableObjects.clear();
	m_Candidates.clear();
	m_PendingCandidateRequests.clear();

	Safe_Release(m_pMainPlayer);
	Safe_Release(m_pGameInstance);

	__super::Free();
}



HRESULT CInteraction_Manager::Load_Data(string SceneName, vector< DefaultInteractionData>& Infos, const string& LoadPath)
{

	

	ifstream file(LoadPath);
	json jInteractionData = json::parse(file);
	for (auto& iInteraction : jInteractionData)
	{
		DefaultInteractionData Data;

		Data.ModelName = iInteraction["ModelName"];
		string InteractionType = iInteraction["InteractionType"];
		if (InteractionType == "CaveRock")
			Data.InteractionType = ENUM_TO_UINT(Interact_Object_Type::CAVEROCK);

		else if (InteractionType == "Rock")
			Data.InteractionType = ENUM_TO_UINT(Interact_Object_Type::ROCK);


		else if (InteractionType == "Lawn")
			Data.InteractionType = ENUM_TO_UINT(Interact_Object_Type::LAWN);


		else if (InteractionType == "Grass")
			Data.InteractionType = ENUM_TO_UINT(Interact_Object_Type::GRASS);

		else if (InteractionType == "Mushroom")
			Data.InteractionType = ENUM_TO_UINT(Interact_Object_Type::ITEM);

		else if (InteractionType == "BigRock")
			Data.InteractionType = ENUM_TO_UINT(Interact_Object_Type::BIGROCK);

		else if (InteractionType == "JackyIronBall")
			Data.InteractionType = ENUM_TO_UINT(Interact_Object_Type::IRONBALL);


		json TransformData = iInteraction["Transform"];

		Data.vPos.x = TransformData["Position"][0].get<float>();
		Data.vPos.y = TransformData["Position"][1].get<float>();
		Data.vPos.z = TransformData["Position"][2].get<float>();

		Data.vRotation.x = TransformData["Rotation"][0].get<float>();
		Data.vRotation.y = TransformData["Rotation"][1].get<float>();
		Data.vRotation.z = TransformData["Rotation"][2].get<float>();

		Data.vScale.x = TransformData["Scale"][0].get<float>();
		Data.vScale.y = TransformData["Scale"][1].get<float>();
		Data.vScale.z = TransformData["Scale"][2].get<float>();


		json ColliderData = iInteraction["Collider"];

		Data.ColliderCenter.x = ColliderData["Center"][0].get<float>();
		Data.ColliderCenter.y = ColliderData["Center"][1].get<float>();
		Data.ColliderCenter.z = ColliderData["Center"][2].get<float>();

		Data.ColliderExtent.x = ColliderData["Extent"][0].get<float>();
		Data.ColliderExtent.y = ColliderData["Extent"][1].get<float>();
		Data.ColliderExtent.z = ColliderData["Extent"][2].get<float>();;

		Infos.push_back(Data);

	}


	return S_OK;
}






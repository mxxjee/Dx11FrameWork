#include "CInteraction_Manager.h"
#include "CIInteractable.h"
#include "CInput_Manager.h"
#include "Client_Defines.h"
#include "CInteractionObject.h"
#include "CPlayer.h"
#include "CGameInstance.h"
#include "CLayer.h"

USING(Client)
IMPLEMENT_SINGLETON(CInteraction_Manager)
void CInteraction_Manager::RegisterInteractable(CIInteractable* pObj)
{
	CIInteractable* pTarget = Find_Object(pObj);

	if (!pTarget)
	{
		m_InteractableObjects.push_back(pObj);
	}
}



void CInteraction_Manager::UnRegisterInteractable(const CIInteractable* pObj)
{
	CIInteractable* Interactables = Find_Object(pObj);
	if (Interactables)
	{
		list<CIInteractable*>::iterator pFindObj = find(m_InteractableObjects.begin(), m_InteractableObjects.end(), pObj);
		if (pFindObj != m_InteractableObjects.end())
			m_InteractableObjects.erase(pFindObj);
	}

	else
		return;
}

void CInteraction_Manager::Update(_float fTimeDelta)
{
	CheckNull(m_pMainPlayer);

	CheckTrue(m_InteractableObjects.empty());
	
	//최적의 InteratableOBject를 찾아서 저장
	CIInteractable* pBest = nullptr;

	//전체 리스트를 돌면서 상호작용가능한 조건을 가지는 애들을 간추리기
	for (auto pInteratable : m_InteractableObjects)
	{
		CheckTrue(m_pMainPlayer->Get_ActionControl()->m_bCarry);
	
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
	CheckNullResult(m_pMainPlayer, false);
	CheckTrueResult(m_pMainPlayer->Get_ActionControl()->m_bItemGet, false);

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

		else
		{
			if (m_pCurrentTarget->Get_Interaction_Priority() != ENUM_TO_UINT(InteractionType::NPC))
			{
				m_pCurrentTarget->Exit_Interaction();
				m_pCurrentTarget->m_bPrevRange = false;
				return true;
			}

		}

		m_pCurrentTarget->Pressed_InteractionKey();

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
	m_InteractableObjects.clear();

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






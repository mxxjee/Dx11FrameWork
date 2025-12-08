#include "CInteraction_Manager.h"
#include "CIInteractable.h"
#include "CInput_Manager.h"
#include "Client_Defines.h"
#include "CInteractionObject.h"
#include "CBounding_AABB.h"
#include "CBoxColliderComponent.h"


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
	CheckNull(MainInteractbles);
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

HRESULT CInteraction_Manager::Load_Data(string SceneName, const string& LoadPath)
{

	vector< DefaultInteractionData>		Infos;

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

	if (FAILED(Create_Object_By_LoadData(SceneName, Infos)))
		return E_FAIL;

	return S_OK;
}


HRESULT CInteraction_Manager::Create_Object_By_LoadData(string SceneName, vector<DefaultInteractionData>& Infos)
{
	CheckTrueResult(Infos.empty(),E_FAIL);


	//넣기 위한 해시값
	size_t Key = hash<string>()(SceneName);
	for (auto& Info : Infos)
	{
		CInteractionObject::Interaction_DESC Desc;
		Desc.eInteractionType = ENUM_TO_UINT(InteractionType::OBJECT);
		Desc.eInteract_Object_Type = Info.InteractionType;

		Desc.eRenderGroup = ENUM_TO_UINT(RENDERGROUP::NONALPHA);
		Desc.ModelName = StringToWString(Info.ModelName);

		Desc.bAnimated = false;

		CTransform::TRANSFORM_DESC TransDesc;
		TransDesc.vLocalPosition = _float4(Info.vPos.x, Info.vPos.y, Info.vPos.z, 1.f);
		TransDesc.vLocalRotation = _float4(Info.vRotation.x, Info.vRotation.y, Info.vRotation.z, 0.f);
		TransDesc.vLocalScale = _float4(Info.vScale.x, Info.vScale.y, Info.vScale.z,1.f);
		Desc.TransformDesc = &TransDesc;


		CBoxColliderComponent::COLLIDER_DESC ColDesc;
		ColDesc.m_eColGroup = ENUM_TO_UINT(COLLISION_GROUP::INTERACTION);
		CBounding_AABB::BOUNDING_AABB_DESC aabbDesc;
		aabbDesc.vCenter = Info.ColliderCenter;
		aabbDesc.Extents = Info.ColliderExtent;
		ColDesc.m_BoundingDesc = &aabbDesc;
		Desc.pColliderComp = &ColDesc;

		//생성을떄려준다
		


		


	}
	
	return S_OK;
}


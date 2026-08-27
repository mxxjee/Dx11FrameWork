#include "CInteraction_Manager.h"
#include "CIInteractable.h"
#include "CInput_Manager.h"
#include "Client_Defines.h"
#include "CInteractionObject.h"
#include "CPlayer.h"
#include "CGameInstance.h"
#include "CLayer.h"
#include "CGameManager.h"
#include "CTransform.h"

#if defined(_DEBUG)
#include <algorithm>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <sstream>
#endif

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
#if defined(_DEBUG)
	using BaselineClock = std::chrono::steady_clock;
	BaselineClock::time_point UpdateBegin = BaselineClock::now();
	const unsigned long long iRegisteredObjects = static_cast<unsigned long long>(m_InteractableObjects.size());
	unsigned long long iInRangeObjects = 0;
	unsigned long long iIsInteractableCalls = 0;
	unsigned long long iPriorityComparisons = 0;
	unsigned long long iRangeEnters = 0;
	unsigned long long iRangeExits = 0;
	double dFirstLoopMicroseconds = 0.0;
	double dSecondLoopMicroseconds = 0.0;
	CIInteractable* pTargetAtFrameStart = m_pCurrentTarget;

	const auto CommitFrame = [&]()
		{
			const double dUpdateMicroseconds = std::chrono::duration<double, std::micro>(BaselineClock::now() - UpdateBegin).count();
			Commit_BaselineFrame(
				iRegisteredObjects,
				iInRangeObjects,
				iIsInteractableCalls,
				iPriorityComparisons,
				iRangeEnters,
				iRangeExits,
				dFirstLoopMicroseconds,
				dSecondLoopMicroseconds,
				dUpdateMicroseconds,
				pTargetAtFrameStart);
		};
#endif

	if (!m_pMainPlayer)
	{
#if defined(_DEBUG)
		++m_BaselineStats.iEarlyOutNoPlayer;
		CommitFrame();
#endif
		return;
	}

#if defined(_DEBUG)
	const auto LogCheckpoint = [](const char* pAction, _uint iLevel, const _float4& vPosition)
		{
			std::ostringstream Stream;
			Stream << std::fixed << std::setprecision(3)
				<< "[InteractionBaselineCheckpoint] action=" << pAction
				<< " level=" << iLevel
				<< " x=" << vPosition.x
				<< " y=" << vPosition.y
				<< " z=" << vPosition.z
				<< " w=" << vPosition.w
				<< '\n';

			const std::string Line = Stream.str();
			OutputDebugStringA(Line.c_str());
			std::ofstream LogFile("Interaction_Baseline.log", std::ios::app);
			if (LogFile)
				LogFile << Line;
		};

	if (CInput_Manager::GetInstance()->IsKeyPressed(KeyCode::F7))
	{
		Dump_BaselineStats();
		UpdateBegin = BaselineClock::now();
	}

	if (CInput_Manager::GetInstance()->IsKeyPressed(KeyCode::F8))
	{
		{
			const std::string Line = "[InteractionBaselineControl] action=reset\n";
			OutputDebugStringA(Line.c_str());
			std::ofstream LogFile("Interaction_Baseline.log", std::ios::app);
			if (LogFile)
				LogFile << Line;
		}

		// Exclude the reset control path and its file I/O from the first measured frame.
		Reset_BaselineStats();
		UpdateBegin = BaselineClock::now();
	}

	if (CInput_Manager::GetInstance()->IsKeyPressed(KeyCode::F9))
	{
		_float4 vPosition{};
		XMStoreFloat4(&vPosition, m_pMainPlayer->Get_Transform()->Get_State(STATE::POSITION));

		std::ofstream CheckpointFile("Interaction_Baseline_Checkpoint.txt", std::ios::trunc);
		if (CheckpointFile)
		{
			CheckpointFile << m_iBaselineActiveLevel << ' '
				<< std::setprecision(9)
				<< vPosition.x << ' ' << vPosition.y << ' ' << vPosition.z << ' ' << vPosition.w << '\n';
		}

		LogCheckpoint("save", m_iBaselineActiveLevel, vPosition);
	}

	if (CInput_Manager::GetInstance()->IsKeyPressed(KeyCode::F10))
	{
		_uint iCheckpointLevel = 0;
		_float4 vPosition{};
		std::ifstream CheckpointFile("Interaction_Baseline_Checkpoint.txt");

		if (CheckpointFile &&
			CheckpointFile >> iCheckpointLevel >> vPosition.x >> vPosition.y >> vPosition.z >> vPosition.w)
		{
			if (iCheckpointLevel == m_iBaselineActiveLevel)
			{
				m_pMainPlayer->Get_Transform()->Set_State(STATE::POSITION, vPosition);
				LogCheckpoint("load", iCheckpointLevel, vPosition);
			}
			else
				LogCheckpoint("skip-level-mismatch", iCheckpointLevel, vPosition);
		}
	}
#endif

	if (m_InteractableObjects.empty())
	{
#if defined(_DEBUG)
		++m_BaselineStats.iEarlyOutEmptyRegistry;
		CommitFrame();
#endif
		return;
	}
	
	// Find the best interactable target for the current frame.
	CIInteractable* pBest = nullptr;

	if (CGameManager::GetInstance()->Get_EndingStep() == EndingStep::EPILOGUE)
	{
#if defined(_DEBUG)
		++m_BaselineStats.iEarlyOutEpilogue;
		CommitFrame();
#endif
		return;
	}

#if defined(_DEBUG)
	const BaselineClock::time_point FirstLoopBegin = BaselineClock::now();
#endif

	// Traverse every registered interactable and preserve the existing priority policy.
	for (auto pInteratable : m_InteractableObjects)
	{
		if (m_pMainPlayer->Get_ActionControl()->m_bCarry)
		{
#if defined(_DEBUG)
			++m_BaselineStats.iEarlyOutCarry;
			dFirstLoopMicroseconds = std::chrono::duration<double, std::micro>(BaselineClock::now() - FirstLoopBegin).count();
			CommitFrame();
#endif
			return;
		}

		if (!pInteratable)
			continue;

#if defined(_DEBUG)
		++iIsInteractableCalls;
#endif
		bool inRange = pInteratable->IsInteratable();

#if defined(_DEBUG)
		if (inRange)
			++iInRangeObjects;
#endif

		// Preserve the existing range lifecycle callbacks.
		if (!pInteratable->m_bPrevRange && inRange)
		{
			pInteratable->Enter_InteractRange();
#if defined(_DEBUG)
			++iRangeEnters;
#endif
		}

		else if (pInteratable->m_bPrevRange && inRange)
			pInteratable->Stay_InteractRange(fTimeDelta);

		else if (pInteratable->m_bPrevRange && !inRange)
		{
			pInteratable->Exit_InteractRange();
#if defined(_DEBUG)
			++iRangeExits;
#endif
		}

		pInteratable->m_bPrevRange = inRange;

		if (inRange)
		{
#if defined(_DEBUG)
			if (pBest)
				++iPriorityComparisons;
#endif
			if (!pBest || pInteratable->Get_Interaction_Priority() > pBest->Get_Interaction_Priority())
			{
				m_pPreTarget = m_pCurrentTarget;
				pBest = pInteratable;
			}
		}
	}

#if defined(_DEBUG)
	dFirstLoopMicroseconds = std::chrono::duration<double, std::micro>(BaselineClock::now() - FirstLoopBegin).count();
#endif

	m_pCurrentTarget = pBest;

	if (m_pCurrentTarget && m_pCurrentTarget->m_bPrevInteracting)
		m_pCurrentTarget->Stay_Interaction(fTimeDelta);

#if defined(_DEBUG)
	const BaselineClock::time_point SecondLoopBegin = BaselineClock::now();
#endif

	for (auto obj : m_InteractableObjects)
	{
		if (obj->m_bPrevInteracting && obj != m_pCurrentTarget)
		{
			obj->Exit_Interaction();
			obj->m_bPrevInteracting = false;
		}
	}

#if defined(_DEBUG)
	dSecondLoopMicroseconds = std::chrono::duration<double, std::micro>(BaselineClock::now() - SecondLoopBegin).count();
	CommitFrame();
#endif
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
#if defined(_DEBUG)
				++m_BaselineStats.iInteractionStarts;
				m_pBaselineInteractionStartTarget = m_pCurrentTarget;
#endif
				return true;
			}

			else
			{
				if (m_pCurrentTarget->Get_Interaction_Priority() != ENUM_TO_UINT(InteractionType::NPC))
				{
					m_pCurrentTarget->Exit_Interaction();
					m_pCurrentTarget->m_bPrevRange = false;
#if defined(_DEBUG)
					++m_BaselineStats.iInteractionEnds;
#endif
					return true;
				}

			}
		}


#if defined(_DEBUG)
		const bool bWasInteracting = m_pCurrentTarget->m_bPrevInteracting;
#endif
		m_pCurrentTarget->Pressed_InteractionKey();
#if defined(_DEBUG)
		if (bWasInteracting && !m_pCurrentTarget->m_bPrevInteracting)
			++m_BaselineStats.iInteractionEnds;
#endif

	}
	
	return false;

}

void CInteraction_Manager::Clear()
{

	// Clean up the active target before changing scenes.
	if (m_pCurrentTarget)
	{
		m_pCurrentTarget->Exit_Interaction();
		m_pCurrentTarget->m_bPrevInteracting = false;
		m_pCurrentTarget->m_bPrevRange = false;
		m_pCurrentTarget = nullptr;
	}

	// Apply the same cleanup to every registered interactable.
	for (auto& pObj : m_InteractableObjects)
	{
		if (pObj)
		{
			// Invoke the matching exit callback for active interaction/range state.
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


}

bool CInteraction_Manager::Check_InteractiveType(InteractionType eType)
{
	CheckNullResult(m_pCurrentTarget, false);

	return m_pCurrentTarget->Get_Interaction_Priority() == (int)eType;
}

void CInteraction_Manager::Add_Interaction(CIInteractable* pObj)
{
	m_InteractableObjects.push_back(pObj);
}

void CInteraction_Manager::Change_Scene(_uint iLevelID)
{
#if defined(_DEBUG)
	using BaselineClock = std::chrono::steady_clock;
	const BaselineClock::time_point ChangeSceneBegin = BaselineClock::now();
	m_iBaselineActiveLevel = iLevelID;
#endif

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
	
#if defined(_DEBUG)
	const double dChangeSceneMicroseconds = std::chrono::duration<double, std::micro>(BaselineClock::now() - ChangeSceneBegin).count();
	Record_ChangeSceneBaseline(dChangeSceneMicroseconds);
#endif

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
#if defined(_DEBUG)
	++m_BaselineStats.iInteractionStarts;
	m_pBaselineInteractionStartTarget = m_pCurrentTarget;
#endif
	
}

void CInteraction_Manager::Free()
{
#if defined(_DEBUG)
	Dump_BaselineStats();
#endif

	m_pCurrentTarget = nullptr;
	m_InteractableObjects.clear();

	Safe_Release(m_pMainPlayer);
	Safe_Release(m_pGameInstance);

	__super::Free();
}

#if defined(_DEBUG)
void CInteraction_Manager::Reset_BaselineStats()
{
	m_BaselineStats = {};
	m_BaselineStats.iMinInRangeObjects = ~0ull;
	m_pBaselineInteractionStartTarget = nullptr;
}

void CInteraction_Manager::Record_CarryNotifyTarget(CIInteractable* pTarget)
{
	++m_BaselineStats.iCarryNotifyCalls;

	if (pTarget != m_pBaselineInteractionStartTarget)
		++m_BaselineStats.iCarryNotifyTargetMismatches;

	m_pBaselineInteractionStartTarget = nullptr;
}

void CInteraction_Manager::Commit_BaselineFrame(
	unsigned long long iRegisteredObjects,
	unsigned long long iInRangeObjects,
	unsigned long long iIsInteractableCalls,
	unsigned long long iPriorityComparisons,
	unsigned long long iRangeEnters,
	unsigned long long iRangeExits,
	double dFirstLoopMicroseconds,
	double dSecondLoopMicroseconds,
	double dUpdateMicroseconds,
	CIInteractable* pTargetAtFrameStart)
{
	++m_BaselineStats.iMeasuredFrames;
	m_BaselineStats.iLatestRegisteredObjects = iRegisteredObjects;
	m_BaselineStats.iMaxRegisteredObjects = (std::max)(m_BaselineStats.iMaxRegisteredObjects, iRegisteredObjects);
	m_BaselineStats.iTotalRegisteredObjects += iRegisteredObjects;
	m_BaselineStats.iLatestInRangeObjects = iInRangeObjects;
	m_BaselineStats.iMinInRangeObjects = (std::min)(m_BaselineStats.iMinInRangeObjects, iInRangeObjects);
	m_BaselineStats.iMaxInRangeObjects = (std::max)(m_BaselineStats.iMaxInRangeObjects, iInRangeObjects);
	m_BaselineStats.iTotalInRangeObjects += iInRangeObjects;
	m_BaselineStats.iIsInteractableCalls += iIsInteractableCalls;
	m_BaselineStats.iPriorityComparisons += iPriorityComparisons;
	m_BaselineStats.iRangeEnters += iRangeEnters;
	m_BaselineStats.iRangeExits += iRangeExits;

	if (pTargetAtFrameStart != m_pCurrentTarget)
		++m_BaselineStats.iCurrentTargetChanges;

	m_BaselineStats.dFirstLoopTotalMicroseconds += dFirstLoopMicroseconds;
	m_BaselineStats.dFirstLoopMaxMicroseconds = (std::max)(m_BaselineStats.dFirstLoopMaxMicroseconds, dFirstLoopMicroseconds);
	m_BaselineStats.dSecondLoopTotalMicroseconds += dSecondLoopMicroseconds;
	m_BaselineStats.dSecondLoopMaxMicroseconds = (std::max)(m_BaselineStats.dSecondLoopMaxMicroseconds, dSecondLoopMicroseconds);
	m_BaselineStats.dUpdateTotalMicroseconds += dUpdateMicroseconds;
	m_BaselineStats.dUpdateMaxMicroseconds = (std::max)(m_BaselineStats.dUpdateMaxMicroseconds, dUpdateMicroseconds);

	if (m_BaselineStats.iMeasuredFrames % 600ull == 0ull)
		Dump_BaselineStats();
}

void CInteraction_Manager::Record_ChangeSceneBaseline(double dMicroseconds)
{
	++m_BaselineStats.iChangeSceneCalls;
	m_BaselineStats.dChangeSceneTotalMicroseconds += dMicroseconds;
	m_BaselineStats.dChangeSceneMaxMicroseconds = (std::max)(m_BaselineStats.dChangeSceneMaxMicroseconds, dMicroseconds);
}

void CInteraction_Manager::Dump_BaselineStats() const
{
	if (m_BaselineStats.iMeasuredFrames == 0)
		return;

	const double dFrames = static_cast<double>(m_BaselineStats.iMeasuredFrames);
	const double dChangeSceneCalls = static_cast<double>(m_BaselineStats.iChangeSceneCalls);
	const unsigned long long iMinInRange =
		m_BaselineStats.iMinInRangeObjects == ~0ull ? 0ull : m_BaselineStats.iMinInRangeObjects;

	std::ostringstream Stream;
	Stream << std::fixed << std::setprecision(3)
		<< "[InteractionBaseline]"
		<< " frames=" << m_BaselineStats.iMeasuredFrames
		<< " latestN=" << m_BaselineStats.iLatestRegisteredObjects
		<< " maxN=" << m_BaselineStats.iMaxRegisteredObjects
		<< " avgN=" << (static_cast<double>(m_BaselineStats.iTotalRegisteredObjects) / dFrames)
		<< " latestK=" << m_BaselineStats.iLatestInRangeObjects
		<< " minK=" << iMinInRange
		<< " maxK=" << m_BaselineStats.iMaxInRangeObjects
		<< " avgK=" << (static_cast<double>(m_BaselineStats.iTotalInRangeObjects) / dFrames)
		<< " isCallsPerFrame=" << (static_cast<double>(m_BaselineStats.iIsInteractableCalls) / dFrames)
		<< " priorityPerFrame=" << (static_cast<double>(m_BaselineStats.iPriorityComparisons) / dFrames)
		<< " firstAvgUs=" << (m_BaselineStats.dFirstLoopTotalMicroseconds / dFrames)
		<< " firstMaxUs=" << m_BaselineStats.dFirstLoopMaxMicroseconds
		<< " secondAvgUs=" << (m_BaselineStats.dSecondLoopTotalMicroseconds / dFrames)
		<< " secondMaxUs=" << m_BaselineStats.dSecondLoopMaxMicroseconds
		<< " updateAvgUs=" << (m_BaselineStats.dUpdateTotalMicroseconds / dFrames)
		<< " updateMaxUs=" << m_BaselineStats.dUpdateMaxMicroseconds
		<< " rangeEnter=" << m_BaselineStats.iRangeEnters
		<< " rangeExit=" << m_BaselineStats.iRangeExits
		<< " targetChanges=" << m_BaselineStats.iCurrentTargetChanges
		<< " interactionStart=" << m_BaselineStats.iInteractionStarts
		<< " interactionEnd=" << m_BaselineStats.iInteractionEnds
		<< " carryNotify=" << m_BaselineStats.iCarryNotifyCalls
		<< " carryTargetMismatch=" << m_BaselineStats.iCarryNotifyTargetMismatches
		<< " changeSceneAvgUs=" << (dChangeSceneCalls > 0.0 ? m_BaselineStats.dChangeSceneTotalMicroseconds / dChangeSceneCalls : 0.0)
		<< " changeSceneMaxUs=" << m_BaselineStats.dChangeSceneMaxMicroseconds
		<< " earlyNoPlayer=" << m_BaselineStats.iEarlyOutNoPlayer
		<< " earlyEmpty=" << m_BaselineStats.iEarlyOutEmptyRegistry
		<< " earlyEpilogue=" << m_BaselineStats.iEarlyOutEpilogue
		<< " earlyCarry=" << m_BaselineStats.iEarlyOutCarry
		<< '\n';

	const std::string Line = Stream.str();
	OutputDebugStringA(Line.c_str());

	std::ofstream LogFile("Interaction_Baseline.log", std::ios::app);
	if (LogFile)
		LogFile << Line;
}
#endif



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






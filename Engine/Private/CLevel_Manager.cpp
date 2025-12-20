#include "CLevel_Manager.h"
#include "CLevel.h"
#include "CGameInstance.h"

CLevel_Manager::CLevel_Manager()
	:m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CLevel_Manager::Level_Changer(_uint iSceneID, LevelArgs& args)
{
	Clear_DestroyStack();

	CLevel* top = nullptr;
	

	CLevel* pNewLevel = Find_And_CreateLevel(iSceneID, args);
	CheckNullResult(pNewLevel, E_FAIL);

	
	LEVELCHANGETYPE eTargetType;
	eTargetType = (args.loadingChangeType == LEVELCHANGETYPE::NONE) ? args.changeType : args.loadingChangeType;

	if (!m_Stack.empty())
	{
		
		PopIfTransient();

		switch (eTargetType)
		{

			//이전씬 삭제.
		case LEVELCHANGETYPE::REPLACETOP:
			ReplaceTop_Level(iSceneID,pNewLevel);
			break;


		case LEVELCHANGETYPE::OVERLAY:
			Overlay_Level(iSceneID, pNewLevel);
			break;

		case LEVELCHANGETYPE::PUSH:
			Push_Level(iSceneID, pNewLevel);
			break;
		}

	}
	
	ActiveTop(pNewLevel, eTargetType);
	m_iCurrentLevelID = iSceneID;

	if (pNewLevel->Is_Cached())
	{
		auto iter = m_Cached.find(iSceneID);
		if(iter==m_Cached.end())
			m_Cached.emplace(iSceneID, pNewLevel);
	}



	return S_OK;
}

void CLevel_Manager::Update_Priority(const _float fTimeDelta)
{ 
	//Clear_DestroyStack();
	CheckTrue(m_Stack.empty());
	m_pGameInstance->Update_Priority_Static(fTimeDelta);
	m_Stack.back()->Update_Priority(fTimeDelta);
}

void CLevel_Manager::Update(const _float fTimeDelta)
{
	CheckTrue(m_Stack.empty());
	m_pGameInstance->Update_Static(fTimeDelta);
	m_Stack.back()->Update(fTimeDelta);
}

void CLevel_Manager::Update_Late(const _float fTimeDelta)
{
	
	CheckTrue(m_Stack.empty());
	m_pGameInstance->Update_Late_Static(fTimeDelta);
	m_Stack.back()->Update_Late(fTimeDelta);


}

void CLevel_Manager::Update_Render(const _float fTimeDelta)
{
	CheckTrue(m_Stack.empty());
	m_pGameInstance->Update_Render_Static(fTimeDelta);
	for (auto& level : m_Stack)
	{
		if (level->Get_State() != LEVELSTATE::HIDDEN)
			level->Update_Render(fTimeDelta);
	}
}

void CLevel_Manager::Render()
{
	CheckTrue(m_Stack.empty());
	for (auto& level : m_Stack)
	{
		if (level->Get_State() != LEVELSTATE::HIDDEN)
			level->Render();
	}

}

CLevel* CLevel_Manager::Get_PrevLevel()
{
	CLevel* pPrev = nullptr;
	if (m_Stack.size() >= 2)
	{
		pPrev = m_Stack[m_Stack.size() - 2];
	}
	return pPrev;
}

CLevel* CLevel_Manager::Get_CurrentLevel()
{
	CLevel* pCurrent = nullptr;

	if(!m_Stack.empty())
		pCurrent = m_Stack.back();

	return pCurrent;
}

void CLevel_Manager::ReplaceTop_Level(_uint iSceneID, CLevel* pNewLevel)
{
	Pop_Level();

}

void CLevel_Manager::Push_Level(_uint iSceneID, CLevel* pNewLevel)
{
	/*가장 위에거만 update돌아감,render도 X,지우기 X*/
	if (!m_Stack.empty())
	{
		_uint iNextLevelID = iSceneID;
 		if (pNewLevel->Get_Flag() == LEVELFLAG::TRANSIENT)
			iNextLevelID = pNewLevel->Get_NextLevelID();

		m_Stack.back()->OnPause(iNextLevelID);
		m_Stack.back()->Set_State(LEVELSTATE::HIDDEN);
	}


}

void CLevel_Manager::Overlay_Level(_uint iSceneID, CLevel* pNewLevel)
{


	/*가장 위에거만 update돌아감,render는 수행m,지우기 X*/
	if (!m_Stack.empty())
	{
		m_Stack.back()->OnPause(iSceneID);
		m_Stack.back()->Set_State(LEVELSTATE::PAUSE);
	}
	//m_pCurrentLevel->Clear();  //ADD : Level->Clear()
}

CLevel* CLevel_Manager::Find_And_CreateLevel(_uint iSceneID, LevelArgs& _Arg)
{
	CLevel* pLevel = Find_Cached(iSceneID);
	if (pLevel)
	{
		return pLevel;
	}

	//캐싱된거 ㅔ없을경우,. 생성
	pLevel = m_pGameInstance->Create_Level(iSceneID, _Arg);


	//캐싱하는 맵일경우
	if (pLevel->Is_Cached())
	{
		Safe_AddRef(pLevel);
		
		//Onenter처리이후에 넣자. 다음에 onresume나와야하니까
		//m_Cached.emplace(iSceneID, pLevel);

	}

	return pLevel;

}

CLevel* CLevel_Manager::Find_Cached(_uint iSceneID)
{
	auto iter = m_Cached.find(iSceneID);
	if (iter == m_Cached.end())
		return nullptr;

	return iter->second;
}

void CLevel_Manager::Pop_Level()
{
	CheckTrue(m_Stack.empty());

	CLevel* pLevel = m_Stack.back();

	m_Stack.back()->OnExit();

	//캐싱하지않는 맵이라면 삭제떄리자
	if (!m_Stack.back()->Is_Cached())
		Safe_Release(pLevel);


	m_Stack.pop_back();
	

	if (!m_Stack.empty())
	{
		m_Stack.back()->Set_State(LEVELSTATE::ACTIVE);
		m_Stack.back()->OnResume(pLevel->Get_LevelID());
		m_iCurrentLevelID = m_Stack.back()->Get_LevelID();
	}

}

void CLevel_Manager::Clear_DestroyStack()
{
	if (!m_tDestroy.empty())
	{
		for (auto& level : m_tDestroy)
		{

			m_pGameInstance->Clear(level->Get_LevelID());
			Safe_Release(level);
		}


		m_tDestroy.clear();
	}
}

void CLevel_Manager::ActiveTop(CLevel* pNewLevel, LEVELCHANGETYPE eChangeType)
{
	
	_uint iPreLevel = 0;
	
	if (!m_Stack.empty())
	{
		if (m_Stack.back())
			iPreLevel = m_Stack.back()->Get_LevelID();

	}

	m_Stack.push_back(pNewLevel);

	if (!m_Stack.empty())
	{
		CLevel* top = m_Stack.back();
		top->Set_State(LEVELSTATE::ACTIVE);

		auto iter = m_Cached.find(pNewLevel->Get_LevelID());
		if (iter==m_Cached.end())
			top->OnEnter();

		else
			top->OnResume(iPreLevel);

	}
	
	
}

void CLevel_Manager::PopIfTransient()
{
	if (m_Stack.back()->Get_Flag() == LEVELFLAG::TRANSIENT)
	{
		//캐싱하지않는 맵이라면 삭제떄리자
		if (!m_Stack.back()->Is_Cached())
			Safe_Release(m_Stack.back());

		m_Stack.pop_back();
	}
}


CLevel_Manager* CLevel_Manager::Create()
{
	return new CLevel_Manager();
}

void CLevel_Manager::Free()
{
	__super::Free();

	for (auto& i : m_Stack)
	{
		Safe_Release(i);
	}


	for (auto& i : m_tDestroy)
	{
		Safe_Release(i);
	}

	for (auto& pair : m_Cached)
	{
		CLevel* pLevel = pair.second;
		int RefCount = 0;

		while (RefCount = Safe_Release(pLevel))
			Safe_Release(pLevel);
	}


	m_Stack.clear();
	m_tDestroy.clear();

	Safe_Release(m_pGameInstance);
	

}

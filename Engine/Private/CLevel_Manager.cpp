#include "CLevel_Manager.h"
#include "CLevel.h"
#include "CGameInstance.h"

CLevel_Manager::CLevel_Manager()
	:m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CLevel_Manager::Level_Changer(_uint iSceneID, CLevel* pNewLevel, LEVELCHANGETYPE eChangeType)
{
	
	CLevel* top = nullptr;

	if (!m_Stack.empty())
	{
		PopIfLoading();

		switch (eChangeType)
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

		case LEVELCHANGETYPE::LOADING:
			m_Stack.back()->Set_State(LEVELSTATE::HIDDEN);
			break;
		}

	}
	
	ActiveTop(pNewLevel,eChangeType);
	m_iCurrentLevelID = iSceneID;


	return S_OK;
}

HRESULT CLevel_Manager::Update(const _float fTimeDelta)
{
	CheckTrueResult(m_Stack.empty(),E_FAIL);
	m_Stack.back()->Update(fTimeDelta);

	return S_OK;
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
	m_Stack.back()->Set_State(LEVELSTATE::HIDDEN);
	m_pGameInstance->Clear(iSceneID);


}

void CLevel_Manager::Overlay_Level(_uint iSceneID, CLevel* pNewLevel)
{


	/*가장 위에거만 update돌아감,render는 수행m,지우기 X*/

	m_Stack.back()->Set_State(LEVELSTATE::PAUSE);
	//m_pCurrentLevel->Clear();  //ADD : Level->Clear()
	m_pGameInstance->Clear(iSceneID);

}

void CLevel_Manager::Pop_Level()
{
	CheckTrue(m_Stack.empty());

	m_Stack.back()->Clear();
	m_pGameInstance->Clear(m_iCurrentLevelID);

	m_tDestroy.push_back(m_Stack.back());
	m_Stack.pop_back();
}

void CLevel_Manager::ActiveTop(CLevel* pNewLevel, LEVELCHANGETYPE eChangeType)
{
	m_Stack.push_back(pNewLevel);

	CLevel* top = m_Stack.back();

	if (eChangeType == LEVELCHANGETYPE::LOADING)
		top->Set_State(LEVELSTATE::LOADING);
	else
		top->Set_State(LEVELSTATE::ACTIVE);

	
}

void CLevel_Manager::PopIfLoading()
{
	if (m_Stack.back()->Get_State() == LEVELSTATE::LOADING)
		Pop_Level();
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

	m_Stack.clear();
	m_tDestroy.clear();

	Safe_Release(m_pGameInstance);
	

}

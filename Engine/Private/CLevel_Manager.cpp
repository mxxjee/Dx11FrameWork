#include "CLevel_Manager.h"
#include "CLevel.h"
#include "CGameInstance.h"

CLevel_Manager::CLevel_Manager()
	:m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CLevel_Manager::Level_Changer(_uint iSceneID, CLevel* pNewLevel)
{
	//ÀÌÀü¾À »èÁ¦.
	if (m_pCurrentLevel)
	{
		m_pCurrentLevel->Clear();  //ADD : Level->Clear()
		m_pGameInstance->Clear(iSceneID);

		Safe_Release(m_pCurrentLevel);

	}

	m_iCurrentLevelID = iSceneID;
	m_pCurrentLevel = pNewLevel;

	if (m_pCurrentLevel)
		m_pCurrentLevel->Initialize();


	return S_OK;
}

HRESULT CLevel_Manager::Update(const _float fTimeDelta)
{
	CheckNullResult(m_pCurrentLevel,E_FAIL);
	m_pCurrentLevel->Update(fTimeDelta);

	return S_OK;
}

void CLevel_Manager::Render()
{
	CheckNull(m_pCurrentLevel);
	m_pCurrentLevel->Render();

}

CLevel_Manager* CLevel_Manager::Create()
{
	return new CLevel_Manager();
}

void CLevel_Manager::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pCurrentLevel);

}

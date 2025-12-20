#include "CLocation_Condition.h"
#include "CGameInstance.h"

USING(Client)
CLocation_Condition::CLocation_Condition()
	:m_eType(LOCATION_TYPE::TYPE_TIRRGER_REGION),
	m_pGameInstance(CGameInstance::GetInstance())

{

}

CLocation_Condition::CLocation_Condition(_uint iTargetID)
	: m_iTargetSceneID(iTargetID),
	m_pGameInstance(CGameInstance::GetInstance())
{
}

CLocation_Condition::~CLocation_Condition()
{
	
}

bool CLocation_Condition::IsSatisfied()
{
    if (m_bVisited)
        return true;


	switch (m_eType)
	{
	case Client::CLocation_Condition::TYPE_TIRRGER_REGION:
		if (m_pGameInstance->Get_CurrentLevelID() == m_iTargetSceneID)
		{
			m_bVisited = true;
			return true;

		}

		break;
	case Client::CLocation_Condition::TYPE_SCENE_ID:
		return m_bVisited;
		break;
	default:
		break;
	}
    return false;
}

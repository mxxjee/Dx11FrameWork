#include "CLevelFactroy.h"
#include "CLevel.h"

USING(Engine)

CLevelFactroy::CLevelFactroy()
{
}

void CLevelFactroy::Register(_uint iSceneID, LevelCreator Creator)
{
	CheckNull(Creator);

	auto iter = m_creators.find(iSceneID);

	/*중복 key값 허용X*/
	if (iter != m_creators.end())
		return;

	m_creators.emplace(iSceneID,Creator);

}

CLevel* CLevelFactroy::Create(_uint iSceneID, LevelArgs& _Arg)
{
	auto it = m_creators.find(iSceneID);
	if (it == m_creators.end())
		return nullptr; 

	else
	{
		return it->second(_Arg);
	}
}




CLevelFactroy* CLevelFactroy::Create()
{
	return new CLevelFactroy();
}

void CLevelFactroy::Free()
{
	__super::Free();

	m_creators.clear();
}

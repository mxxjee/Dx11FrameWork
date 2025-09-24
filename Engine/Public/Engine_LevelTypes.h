#pragma once
#include "Engine_Define.h"

namespace Engine
{
	struct LevelArgs
	{
		_uint			iNextLevelID;
		LEVELCHANGETYPE changeType = LEVELCHANGETYPE::REPLACETOP;//이 씬이 어떻게 쌓일건지
		LEVELCHANGETYPE loadingChangeType = LEVELCHANGETYPE::NONE;		//로딩씬일 경우에만 설정

		unsigned int m_iLevelID = 0;
		LEVELSTATE	m_eState = { LEVELSTATE::ACTIVE };
		LEVELFLAG  m_eFlag = { LEVELFLAG::NORMAL };

		//int         roomId = 0;

	};

	class CLevel;
	using LevelCreator = std::function<CLevel* (LevelArgs&)>;


}
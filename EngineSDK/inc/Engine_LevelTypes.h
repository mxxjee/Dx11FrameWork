#pragma once
#include "Engine_Define.h"

namespace Engine
{
	struct LevelArgs
	{
		_uint			iNextLevelID;
		LEVELCHANGETYPE changeType = LEVELCHANGETYPE::REPLACETOP;

		LEVELSTATE	m_eState = { LEVELSTATE::ACTIVE };
		LEVELFLAG  m_eFlag = { LEVELFLAG::NORMAL };

		//int         roomId = 0;

	};

	class CLevel;
	using LevelCreator = std::function<CLevel* (LevelArgs&)>;


}
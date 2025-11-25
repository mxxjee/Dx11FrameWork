#pragma once
#include "Engine_Define.h"

inline GameEvent MakeEvent(const string& name)
{
	GameEvent event;
	event.Name = name;

	return event;

}
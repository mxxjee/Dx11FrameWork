#pragma once
#include "Engine_Define.h"

struct EffectData
{
	_float4 vColor = _float4(1.f, 1.f, 1.f, 1.f);
	_float fLifeTime = 5.f;
	_float fSpeed = 3.f;

	bool  m_bLoop = false;		//루프가 끝나면 자동으로 disalbe
	
	_float4		InitOffSet = _float4(0.f, 0.f, 0.f, 1.f);
	_float4		InitRotation = _float4(0.f, 0.f, 0.f, 0.f);
	_float4		InitScale = _float4(0.f, 0.f, 0.f, 1.f);


};

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
	_float4		InitScale = _float4(1.f, 1.f, 1.f, 1.f);

	bool	bUseScaleAnim = false;
	_float4 TargetScale = { 1.f,1.f,1.f,1.f };
	_float  fScaleSpeed = 1.0f;

	bool    bUseRotationAnim = false;
	_float4 vRotationAxis = { 0.f, 0.f, 1.f,1.f };
	_float  fRotationSpeed = 0.0f;

	bool    bUseMoveAnim = false;
	_float4 vMoveDir = { 0.f, 1.f, 0.f,0.f }; 
	_float  fMoveSpeed = 0.0f;
	
	_float  fIntensity = 1.f;


};

enum EFFECT_TYPE
{
	//플레이어일반공격
	SLASH1,
	SLASH_CHARGE_ST,
	SLASH_CHARGE_COMPLETE,
	SLASH2,//차징공격
	SLASHTRAIL,//일반공격트레일
	HITSPARKLE,
	GUARDEFFECT,
	EFFECT_TYPE_END
};


#pragma once
#include "Engine_Define.h"


struct EffectData
{
    // 1. 큰 덩어리 (16바이트들) - 가장 먼저 배치
    _float4 vColor = _float4(1.f, 1.f, 1.f, 1.f);
    _float4 InitOffSet = _float4(0.f, 0.f, 0.f, 1.f);
    _float4 InitRotation = _float4(0.f, 0.f, 0.f, 0.f);
    _float4 InitScale = _float4(1.f, 1.f, 1.f, 1.f);
    _float4 TargetScale = { 1.f, 1.f, 1.f, 1.f };
    _float4 vRotationAxis = { 0.f, 0.f, 1.f, 1.f };
    _float4 vMoveDir = { 0.f, 1.f, 0.f, 0.f };

    // 2. 4바이트 데이터들
    _float fLifeTime = 5.f;
    _float fSpeed = 3.f;
    _float fScaleSpeed = 1.0f;
    _float fRotationSpeed = 0.0f;
    _float fMoveSpeed = 0.0f;
    _float fIntensity = 1.f;

    // 3. 1바이트 데이터들 (가급적 마지막에 몰아넣기)
    bool m_bLoop = false;
    bool bUseScaleAnim = false;
    bool bUseRotationAnim = false;
    bool bUseMoveAnim = false;

    // 4. 구조체 전체 크기를 16의 배수로 맞추기 위한 패딩 (선택사항이나 권장)
    // _float pad[3];


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


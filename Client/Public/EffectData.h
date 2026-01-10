#pragma once
#include "Engine_Define.h"

enum class EFFECT_DESC_TYPE{MESH,PARTICLE,END};

enum class EFFECT_TEXTYPE{DIFFUSE,NOISE,ALPHAMASK,END};
//공통데이터
struct EffectData
{
    virtual ~EffectData() = default;
    EFFECT_DESC_TYPE eType = EFFECT_DESC_TYPE::MESH; // 타입

    //공통 변수
    string     DataName = "";

    _float4 vColor = _float4(1.f, 1.f, 1.f, 1.f);
    _float4 InitOffSet = _float4(0.f, 0.f, 0.f, 1.f);
    _float4 InitRotation = _float4(0.f, 0.f, 0.f, 0.f);
    _float4 InitScale = _float4(1.f, 1.f, 1.f, 1.f);

    _float  fLifeTime = 1.0f;
    _float fSpeed = 3.f;
    _float fIntensity = 1.f;

    _bool   m_bLoop = false;

};
struct MeshEffectData :public EffectData
{
    MeshEffectData() { eType = EFFECT_DESC_TYPE::MESH; }
    //16바이트
    _float4 TargetScale = { 1.f, 1.f, 1.f, 1.f };
    _float4 vRotationAxis = { 0.f, 0.f, 1.f, 1.f };
    _float4 vMoveDir = { 0.f, 1.f, 0.f, 0.f };

    // 4
    _float fScaleSpeed = 1.0f;
    _float fRotationSpeed = 0.0f;
    _float fMoveSpeed = 0.0f;

    // 1 
    bool bUseScaleAnim = false;
    bool bUseRotationAnim = false;
    bool bUseMoveAnim = false;

    // 패딩 (선택사항이나 권장)
    // _float pad[3];


};

struct ParticleData :public EffectData
{
    ParticleData() { eType = EFFECT_DESC_TYPE::PARTICLE; }

    _uint   iNumInstance = 10;
    _float3 vRange = { 1.f, 1.f, 1.f };
    
    //안쓰고싶으면 start와 end를 같은갑승로 넣는다.
    //기존:_float2 vSizeRange = { 0.1f, 0.5f };   // Min, Max

    _float2 vSizeRange_Start = { 0.1f,0.3f };//생성될 때 크기 (Min, Max)
    _float2 vSizeRange = { 0.5f, 0.8f };   // 죽을 때 크기 (Min, Max)


    _float2 vSpeedRange = { 1.f, 5.f };    // Min, Max
    _float2 vLifeTimeRange = { 1.f, 2.f }; // Min, Max
    _float  fGravity = 0.f;


    //0:Diffuse
    //
    wstring TexKey[ENUM_TO_UINT(EFFECT_TEXTYPE::END)] = {};

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


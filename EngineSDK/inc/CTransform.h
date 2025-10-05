#pragma once
#include "CComponent.h"

NS_BEGIN(Engine)
class ENGINE_DLL CTransform :
    public CComponent
{
public:
    typedef struct tagTransformDesc
    {
        _float4 vLocalPosition = {0.f,0.f,0.f,1.f};
        _float4 vLocalScale = { 1.f,1.f,1.f,1.f };
        _float4 vLocalRotation = { 0.f,0.f,0.f,1.f };

        _float fSpeedPerSec = {};
        _float fRotationPerSec = {};

        _float4x4  m_WorldMatrix;

    }TRANSFORM_DESC;

private:
    CTransform(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CTransform(const CTransform& Prototype);
    virtual ~CTransform() = default;

public:
    virtual         HRESULT     Initialize_Prototype() override;
    virtual         HRESULT     Initialize_Copytype(void* pArg) override;


public:
    //State를 연산용데이터로 얻어오기
    _vector Get_State(STATE eState)
    {
        //mat행렬 전체를 연산용 행렬로 만든 뒤, 그 중 eState번째 행만 가져오기
        return XMLoadFloat4x4(&m_WorldMatrix).r[ENUM_TO_UINT(eState)];
    }

    //저장용데이터 -> 저장용행렬
    void    Set_State(STATE eState, const _float4& vState)
    {
        memcpy(&m_WorldMatrix.m[ENUM_TO_UINT(eState)], &vState, sizeof(_float4));
    }

    //연산용데이터를 저장용행렬에 저장
    void    Set_State(STATE eState, _vector vState)
    {
        XMStoreFloat4((_float4*)&m_WorldMatrix.m[ENUM_TO_UINT(eState)], vState);
    }

    void  Set_Scale(_float4 vScale);
   

    const _float4x4& Get_World() { return m_WorldMatrix; }
    _float3  Get_Scale();        //right.up.forward벡터 길이만 리턴하자.
    _vector  Get_SRT(SRTType eType);

#pragma region Translation
public:
    void    Move(DIRECTION eDir, float fTimeDelta,Space space=Space::Local);
    
    void    Rotation(_fvector vAxis, _float fRadian);       //즉각회전,vAxis축을 기준으로 fRAdian만큼 회전시킨다. 
    void    Rotation(_float3 fEularDegree);
    
    void    Turn(_fvector vAxis, _float fTimeDelta);  //누적회전,매프레임마다 vAxis축을 기준으로 회전

    void    LookAt(_fvector vWorldPoint);    //즉각회전,한 점을 바로 바라보도록회전

    void    LookAt(_fvector vAxis, _fvector vWorldPoint, _float fTimeDelta, _float fSpeed = 5); //누적 회전, 특정 축을 기준으로 vWorldpoint를 바라보도록 회전한다.
    
    void    LookAt(CTransform* target);     

    void    Chase(_fvector vPoint, _float fTimeDelta, _float MinDistance = 0.f);    //최소 거리 까지만 쫓아간다.

#pragma endregion
private:
    /*저장용 데이터( 직렬 계산)*/
    _float  m_fSpeedPerSec = {};
    _float  m_fRotationPerSec = {};
    _float4x4  m_WorldMatrix = {};



public:
    static CTransform* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    virtual     CComponent* Clone(void* pArg) override;
    virtual     void        Free() override;


};
NS_END


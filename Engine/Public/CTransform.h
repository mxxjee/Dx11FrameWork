#pragma once
#include "CComponent.h"

NS_BEGIN(Engine)
class ENGINE_DLL CTransform :
    public CComponent
{
public:
    typedef struct tagTransformDesc : CComponent::tagComponentDesc
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
    //매프레임마다 m_pParent를 통해 부모행렬 계산
    void        Update_Matrix();
public:
    //State를 연산용데이터로 얻어오기
    _vector Get_State(STATE eState, TransformScope eScope=TransformScope::LOCAL);

    //저장용데이터 -> 저장용행렬
    void    Set_State(STATE eState, const _float4& vState)
    {
        memcpy(&m_LocalWorldMatrix.m[ENUM_TO_UINT(eState)], &vState, sizeof(_float4));
    }

    //연산용데이터를 저장용행렬에 저장
    void    Set_State(STATE eState, _vector vState)
    {
        XMStoreFloat4((_float4*)&m_LocalWorldMatrix.m[ENUM_TO_UINT(eState)], vState);
    }

    //강제 WorldMatrix대입
    void  Set_WorldMatrix(const _float4x4& Mat, TransformScope eScope=TransformScope::LOCAL);

    void  Set_Scale(_float4 vScale);

    //부모 설정, setParent할때 local scale유지 여부
    void  Set_Parent(CTransform* pParent);

public:
    //셰이더에게 데이터전송
    HRESULT             Bind_ShaderResource(class CShader* pShader, const string& Variable);
public:
    const _float4x4&        Get_World(TransformScope eScope = TransformScope::LOCAL);
    _matrix                 Get_WorldInverse(TransformScope eScope = TransformScope::LOCAL);
    
    _float3                 Get_Scale_ByFloat3();           //right.up.forward벡터 길이만 리턴하자.
    _vector                 Get_Scale_ByVector();

    _vector                 Get_SRT(SRTType eType);
    CTransform*             Get_Parent() { return m_pParent; }

#pragma region Translation(모두 로컬기준 이동/회전)
public:
    void    Move(DIRECTION eDir, float fTimeDelta,Space space=Space::Local);                                                                  //look벡터 갱신여부
  
    void    Rotation(_vector vAxis, _float fRadian);       //즉각회전,vAxis축을 기준으로 fRAdian만큼 회전시킨다. 
    void    Rotation(_float3 fEularDegree);
    void    AddRotation(_float3 fEularDegree);              //누적회전 , 기존의 회전에 더함

    void    Turn(_vector vAxis, _float fTimeDelta);  //누적회전,매프레임마다 vAxis축을 기준으로 회전
    
    void    LookAt(_vector vWorldPoint);    //즉각회전,한 점을 바로 바라보도록회전
    void    LookAt(_vector vAxis, _vector vWorldPoint, _float fTimeDelta, _float fSpeed = 5); //누적 회전, 특정 축을 기준으로 vWorldpoint를 바라보도록 회전한다.
    void    LookAt(CTransform* target);
    
    void    LookAtWithUpVector(_vector vWorldPoint, _vector vUp);
    
    void    LookAtSmooth(_vector vTargetPos, float fLerpSpped, float fTimeDelta);
  
       
    void    Chase(_vector vPoint, _float fTimeDelta, _float MinDistance = 0.f);    //최소 거리 까지만 쫓아간다.

//////////////////////////
    void    MoveLerp(_vector vTargetPos, float fLerpSpeed, float fTimeDelta, bool bUpdateLook = true);
    void    ScaleLerp(_vector vTargetScale, float fLerpSpeed, float fTimeDelta);
    void    RotateLerp(_vector vTargetRot, float fLerpSpeed, float fTimeDelta);
#pragma endregion

private:
    /*저장용 데이터( 직렬 계산)*/
    _float  m_fSpeedPerSec = {};
    _float  m_fRotationPerSec = {};
    //누적회전을 위한 각도
    _float3   m_fEularDegree = { 0.f,0.f,0.f };


private:
    CTransform* m_pParent = { nullptr };

    _float4x4   m_LocalWorldMatrix = {};//로컬 상태의 matrix
    _float4x4   m_WorldMatrix = {};     //부모행렬까지 모두 계산된 matrix


public:
    static CTransform* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    virtual     CComponent* Clone(void* pArg) override;
    virtual     void        Free() override;


};
NS_END


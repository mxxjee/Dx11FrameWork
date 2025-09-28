#pragma once
#include "CComponent.h"

NS_BEGIN(Engine)
class ENGINE_DLL CTransform :
    public CComponent
{
public:
    typedef struct tagTransformDesc
    {
        _float fSpeedPerSec;
        _float fRotationPerSec;
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

public:
    void    Move(DIRECTION eDir, float fTimeDelta);


private:
    void    Move_Forward(_vector& vPosition,_float fTimeDelta);
    void    Move_Backward(_vector& vPosition,_float fTimeDelta);
    void    Move_Left(_vector& vPosition,_float fTimeDelta);
    void    Move_Right(_vector& vPosition,_float fTimeDelta);
    void    Move_Up(_vector& vPosition,_float fTimeDelta);
    void    Move_Down(_vector& vPosition,_float fTimeDelta);

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


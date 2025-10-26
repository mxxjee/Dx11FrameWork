#pragma once
#include "CComponent.h"

NS_BEGIN(Engine)
class ENGINE_DLL CCollider_Base:
    public CComponent
{
public:
    enum class COLLIDER_TYPE
    {
        SPHERE,
        AABB,
        MESH,
        END
    };
    
public:
    typedef struct tagColliderDesc :CComponent::COMPONENT_DESC
    {
        _float3 Offset = { 0.f,0.f,0.f };       //0,0,0부터 떨어진거리
        _float3 vScaleOffSet = { 1.f,1.f,1.f };       //원래 오브젝트 스케일에 곱해질 수 

    }COLLIDER_DESC;


protected:
    CCollider_Base(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CCollider_Base(const CCollider_Base& Prototype);
    virtual ~CCollider_Base() = default;


public:
    virtual         HRESULT     Initialize_Prototype();
    virtual         HRESULT     Initialize_Copytype(void* pArg);

public:
    virtual      HRESULT   Update_Collider(class CTransform* pTransform)=0;
    virtual      bool   Intersects_Ray(_vector origin, _vector rayDir, _float& Dist)=0;



public:
    virtual CComponent* Clone(void* pArg) = 0;
    virtual void Free() override;

protected:
    COLLIDER_TYPE m_eType;      //콜라이더 타입,어느충돌?
    _matrix             m_WolrdMatrix;      //오너의 월드매트릭스
    _bool               m_bActive = true;   //활성화 여부
    _bool               m_bDebugDraw = false;


  

    _float3                 vOffset = { 0.f,0.f,0.f };      //중심(0,0,0)으로부터 떨어진거리
    _float3                 vScaleOffSet = { 1.f,1.f,1.f };       //충돌체 크기
    _float                  fRadius = 0.5f;             //충돌체반경(구)


    _float3             vCenter = { 0.f,0.f,0.f };


    
};
NS_END


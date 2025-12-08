#pragma once
#include "CCollider_Base.h"

NS_BEGIN(Engine)
class ENGINE_DLL  COBBColliderComponent:
    public CCollider_Base
{
protected:
    COBBColliderComponent(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    COBBColliderComponent(const COBBColliderComponent& Prototype);
    virtual ~COBBColliderComponent() = default;

public:
    virtual         HRESULT     Initialize_Prototype();
    virtual         HRESULT     Initialize_Copytype(void* pArg);

public:
    virtual HRESULT        Update_Collider(XMMATRIX    WorldMatrix);
    bool            Intersects_Ray(_vector origin, _vector rayDir, _float & Dist, class CTransform* pTransform);
 
public:
    bool       Push_Collision(CCollider_Base* pOther, _float3& vOutPush) { return false; }
public:
    void            Set_OffSet(_float3 vOffset);
    void            Set_Size(_float3 vSize);
public:
    static COBBColliderComponent* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    virtual COBBColliderComponent* Clone(void* pArg);
    virtual void Free() override;

};
NS_END

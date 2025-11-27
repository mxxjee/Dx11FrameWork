#pragma once
#include "CCollider_Base.h"

NS_BEGIN(Engine)
class ENGINE_DLL CSphereColliderComponent :
    public CCollider_Base
{

protected:
    CSphereColliderComponent(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CSphereColliderComponent(const CSphereColliderComponent& Prototype);
    virtual ~CSphereColliderComponent() = default;

public:
    virtual         HRESULT     Initialize_Prototype();
    virtual         HRESULT     Initialize_Copytype(void* pArg);

public:
    virtual HRESULT        Update_Collider(class CTransform* pTransform);
    bool            Intersects_Ray(_vector origin, _vector rayDir, _float& Dist, CTransform* pTransform);

public:
    static CSphereColliderComponent* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    virtual CSphereColliderComponent* Clone(void* pArg);
    virtual void Free() override;

public:
    void        Set_Radius(_float fRadius);

};

NS_END
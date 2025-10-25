#pragma once
#include "CComponent.h"

NS_BEGIN(Engine)
class ENGINE_DLL CSphereColliderComponent :
    public CComponent
{
public:
    typedef struct tagBoundingSphereDesc :CComponent::COMPONENT_DESC
    {
        _float3 Center;
        _float Radius;

    }BOUNDINGSPHERE_DESC;

protected:
    CSphereColliderComponent(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CSphereColliderComponent(const CSphereColliderComponent& Prototype);
    virtual ~CSphereColliderComponent() = default;

public:
    virtual         HRESULT     Initialize_Prototype();
    virtual         HRESULT     Initialize_Copytype(void* pArg);

public:
    HRESULT        UpdateCollider(class CTransform* pTransform);
    bool            Intersects(_vector origin, _vector rayDir, _float& Dist);

public:
    virtual CSphereColliderComponent* Clone(void* pArg);
    virtual void Free() override;

public:
    static CSphereColliderComponent* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);

private:
    BoundingSphere      m_Sphere;

};

NS_END
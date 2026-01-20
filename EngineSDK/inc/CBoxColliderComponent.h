#pragma once
#include "CCollider_Base.h"


NS_BEGIN(Engine)
class ENGINE_DLL CBoxColliderComponent :
    public CCollider_Base
{
protected:
    CBoxColliderComponent(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CBoxColliderComponent(const CBoxColliderComponent& Prototype);
    virtual ~CBoxColliderComponent() = default;

public:
    virtual         HRESULT     Initialize_Prototype();
    virtual         HRESULT     Initialize_Copytype(void* pArg);

public:
    virtual HRESULT        Update_Collider(XMMATRIX    WorldMatrix);
    bool            Intersects_Ray(_vector origin, _vector rayDir, _float & Dist, class CTransform* pTransform);

public:
    bool       Push_Collision(CCollider_Base* pOther, _float3& vOutPush);


public:
    void            Set_OffSet(_float3 vOffset);
    void            Set_Size(_float3 vSize);

#ifdef _DEBUG
    virtual void OnInspectorUI();
#endif // _DEBUG

public:
    static CBoxColliderComponent* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    virtual CBoxColliderComponent* Clone(void* pArg);
    virtual void Free() override;


};
NS_END


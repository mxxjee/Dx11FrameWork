#pragma once
#include "CCollider_Base.h"

/*메쉬 픽킹을 위한 Collider*/
/*AABB검사 이후 -> Trianglelist검사*/
NS_BEGIN(Engine)
class CGameInstance;
class CModel;

class ENGINE_DLL CMeshColliderComponent :
    public CCollider_Base
{

protected:
    CMeshColliderComponent(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CMeshColliderComponent(const CMeshColliderComponent& Prototype);
    virtual ~CMeshColliderComponent() = default;

public:
    virtual         HRESULT     Initialize_Prototype();
    virtual         HRESULT     Initialize_Copytype(void* pArg);


public:
    virtual HRESULT        Update_Collider(XMMATRIX    WorldMatrix);
    bool                Intersects_Ray(_vector origin, _vector rayDir, _float& Dist,class CTransform* pTransform);

public:
    bool       Push_Collision(CCollider_Base* pOther, _float3& vOutPush) { return false; }

public:
    virtual CMeshColliderComponent* Clone(void* pArg);
    virtual void Free() override;

public:
    static CMeshColliderComponent* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);

private:
    //바운딩박스로 먼저 검사 + model검사 , 콜라이더 2개필요
    CBounding* m_pMeshBounding = { nullptr };

private:
    CGameInstance* m_pGameInstance = nullptr;
    ENGINE_DESC m_EngineDesc;



    // CCollider_Base을(를) 통해 상속됨
    virtual bool Intersect(CCollider_Base* pOther) override;

};

NS_END


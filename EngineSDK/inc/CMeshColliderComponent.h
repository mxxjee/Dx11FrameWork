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
public:
    typedef struct tagMeshColliderDesc :CCollider_Base::COLLIDER_DESC
    {
        CModel* pModel;

    }COLLIDER_MESH;
protected:
    CMeshColliderComponent(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CMeshColliderComponent(const CMeshColliderComponent& Prototype);
    virtual ~CMeshColliderComponent() = default;

public:
    virtual         HRESULT     Initialize_Prototype();
    virtual         HRESULT     Initialize_Copytype(void* pArg);


public:
    virtual HRESULT        Update_Collider(class CTransform* pTransform);
    bool                Intersects_Ray(_vector origin, _vector rayDir, _float& Dist);


public:
    virtual CMeshColliderComponent* Clone(void* pArg);
    virtual void Free() override;

public:
    static CMeshColliderComponent* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);

private:
    BoundingBox      m_BoundingBox;
    CModel*           m_pModel=nullptr;

private:
    CGameInstance* m_pGameInstance = nullptr;
    ENGINE_DESC m_EngineDesc;


};

NS_END


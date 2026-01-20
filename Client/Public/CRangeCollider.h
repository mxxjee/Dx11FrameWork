#pragma once
#include "CPartObject.h"
namespace Engine
{
    class CSphereColliderComponent;
}

NS_BEGIN(Client)
class CRangeCollider :
    public CPartObject
{
public:
    typedef struct tagRangeColliderDesc : public CPartObject::PARTOBJECT_DESC
    {
        _float fRadius = {};


    }RANGE_COLLIDER_DESC;


protected:
    CRangeCollider(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CRangeCollider(const CRangeCollider& Prototype);
    virtual ~CRangeCollider() = default;

public:
    virtual HRESULT     Initialize_Prototype(); /*원형 객체가 생성될때 부르는 Initialize*/
    virtual HRESULT     Initialize_Copytype(void* pArg); /*사본 객체가 생성될때 부르는 Initialize*/

    virtual void        Update_Priority(_float fTimeDelta);
    virtual void        Update(_float fTimeDelta);
    virtual void        Update_Late(_float fTimeDelta);
    virtual void        Update_Render(_float fTimeDelta);

    virtual HRESULT Render();

public:
    void        Register_Colliders(_uint iLevelIdx);

private:
    HRESULT         Ready_Component(void* pArg);

public:
    virtual void    Set_Active(bool _b);
    bool            Is_Collision() { return m_bCollision; }
    void            Set_Radius(_float radius);

protected:
    CSphereColliderComponent* m_pCollider = { nullptr };

public:
    CSphereColliderComponent* Get_Collider() { return m_pCollider; }
protected:
    const _float4x4* m_pSocketMatrix = { nullptr };

public:
    virtual     void    OnCollisionEnter(_uint iGroup, CCollider_Base * pOther);
    virtual     void    OnCollisionStay(_uint iGroup, CCollider_Base * pOther);
    virtual     void    OnCollisionExit(_uint iGroup, CCollider_Base * pOther);

public:
    static CRangeCollider* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;

private:
    bool        m_bCollision = false;
};
NS_END

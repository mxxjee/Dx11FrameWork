#pragma once
#include "CGameObject.h"

namespace Engine
{
    class CCollider_Base;

}

NS_BEGIN(Client)

class CWall :
    public CGameObject
{
public:
    typedef struct tagWallDesc :public CGameObject::GAMEOBJECT_DESC
    {
        _float3     vExtents;
    }WALL_DESC;


protected:
    CWall(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CWall(const CWall& Prototype);
    virtual ~CWall() = default;

public:
    virtual HRESULT     Initialize_Prototype(); /*원형 객체가 생성될때 부르는 Initialize*/
    virtual HRESULT     Initialize_Copytype(void* pArg); /*사본 객체가 생성될때 부르는 Initialize*/

    virtual void        Update_Priority(_float fTimeDelta);
    virtual void        Update(_float fTimeDelta);
    virtual void        Update_Late(_float fTimeDelta);
    virtual void        Update_Render(_float fTimeDelta);
    virtual HRESULT Render();

public:
    virtual     void    OnCollisionEnter(_uint iGroup, CCollider_Base* pOther);
    virtual     void    OnCollisionStay(_uint iGroup, CCollider_Base* pOther);
    virtual     void    OnCollisionExit(_uint iGroup, CCollider_Base* pOther);


private:
    HRESULT         Ready_Component(void* pArg);

private:
    CCollider_Base* m_pCollider = nullptr;
    
public:
    static CWall* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;

};

NS_END


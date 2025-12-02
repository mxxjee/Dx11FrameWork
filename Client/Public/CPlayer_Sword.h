#pragma once
#include "CWeapon.h"

NS_BEGIN(Client)
class CPlayer_Sword :
    public CWeapon
{
    enum CollisionPos{BOTTON,TOP,END};
protected:
    CPlayer_Sword(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CPlayer_Sword(const CPlayer_Sword& Prototype);
    virtual ~CPlayer_Sword() = default;

    

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
    HRESULT Ready_Components(void* pArg);
public:
    static CPlayer_Sword* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;


};

NS_END

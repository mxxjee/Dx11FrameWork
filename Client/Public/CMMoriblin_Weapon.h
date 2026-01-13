#pragma once
#include "CWeapon.h"

NS_BEGIN(Client)
class CM_MoriblinSword;
class CMMoriblin_Weapon :
    public CWeapon
{
public:
    typedef struct tagMoriblinWeapon : CWeapon::WEAPON_DESC
    {
        void* pCollider = nullptr;

        float fRadius = 0.3f;
        _float3 vOffSet;

        wstring AnimKey;        //패링 애니메이션
    }MORIBLIN_WEAPON;



protected:
    CMMoriblin_Weapon(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CMMoriblin_Weapon(const CMMoriblin_Weapon& Prototype);
    virtual ~CMMoriblin_Weapon() = default;

public:
    virtual HRESULT     Initialize_Prototype(); /*원형 객체가 생성될때 부르는 Initialize*/
    virtual HRESULT     Initialize_Copytype(void* pArg); /*사본 객체가 생성될때 부르는 Initialize*/

    virtual void        Update_Priority(_float fTimeDelta);
    virtual void        Update(_float fTimeDelta);
    virtual void        Update_Late(_float fTimeDelta);
    virtual void        Update_Render(_float fTimeDelta);

    virtual HRESULT Render();

private:
    HRESULT Ready_Components(void* pArg);

public:
    static CMMoriblin_Weapon* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;

public:
    virtual     void    OnCollisionEnter(_uint iGroup, CCollider_Base* pOther);
    virtual     void    OnCollisionStay(_uint iGroup, CCollider_Base* pOther);
    virtual     void    OnCollisionExit(_uint iGroup, CCollider_Base* pOther);


private:
    void        PlayGuardEffect(_matrix Matrix);
private:
    float m_fRadius = 0.f;
    _float3 m_vOffSet;
    wstring m_AnimKey = L"";

private:
    CM_MoriblinSword* m_pMoriblin = nullptr;
    CGameObject::GAMEOBJECT_DESC* pGuardEffect = nullptr;


};
NS_END


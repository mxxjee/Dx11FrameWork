#pragma once

#include "CMonster.h"

NS_BEGIN(Client)
class CM_MoriblinSword :
    public CMonster
{
public:
    enum MoriblinState 
    {
        DETECT = 10, GUARD, END
    };
protected:
    CM_MoriblinSword(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CM_MoriblinSword(const CM_MoriblinSword& rhs);
    virtual ~CM_MoriblinSword();


public:
    void                    Register_Anim();
    HRESULT                 Ready_Component(void* pArg);
    HRESULT                Ready_States();
    HRESULT                Ready_WeaponColliders();
public:
    virtual string Convert_String_To_Enum(_uint eState);

private:
    void        Idle_Behavior(float fTimeDelta);
    void        Chase_Behavior(float fTimeDelta);
    void        Attack_Behavior(float fTimeDelta);
    void        Patrol_Behavior(float fTimeDelta);

    virtual void        Set_CollisionEnable(bool _b);

public:
    virtual void        Enter_State(int newState);
    virtual void        Exit_State(int newState);
    virtual void        AIState_Change(_float fTimeDelta) override;
    virtual void        Update_Movement(_float fTimeDelta) override;

public:
    virtual HRESULT     Initialize_Prototype(); /*원형 객체가 생성될때 부르는 Initialize*/
    virtual HRESULT     Initialize_Copytype(void* pArg); /*사본 객체가 생성될때 부르는 Initialize*/

    virtual void        Update_Priority(_float fTimeDelta);
    virtual void        Update(_float fTimeDelta);
    virtual void        Update_Late(_float fTimeDelta);
    virtual void        Update_Render(_float fTimeDelta) override;
    virtual HRESULT     Render();

public:
    static CM_MoriblinSword* Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;

public:
    virtual void        Set_Dead();
public:
    virtual     void    OnCollisionEnter(_uint iGroup, CCollider_Base* pOther);
    //플레이어 weapon과 충돌했을때 밀어내는 행동(재정의 가능)
    virtual void        Push_Behavior(CGameObject* pOther);
public:
    bool        Get_Detect() { return m_bDetect; }
    bool        Get_Guard() { return m_bGuard; }
    void        Set_Guard(bool b, wstring AnimKey);//가드세팅과 동시에 애니멩시ㅕㄴ 지정
private:
    bool            m_bDetect = false;      //이전에 감지했던 기록이없고, idle상태일때 전이가능
    _float          m_fDetectRange = 10.f;
    bool            m_bGuard = false;

    float           m_fReachTime = 0.f;//만약 3초이상 도달못할경우 강제로 nextcell바꾸기

private:
    vector<class CWeapon*>        m_pWeapons;
    _uint                       m_iEventHandle = 0;

};
NS_END

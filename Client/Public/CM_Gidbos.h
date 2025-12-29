#pragma once
#include "CMonster.h"


NS_BEGIN(Client)

class CRangeCollider;

class CM_Gidbos :
    public CMonster
{
public:
    typedef struct GidbosDesc : public CMonster::MONSTER_DESC
    {
        bool        m_StartLeft = false;
        _float3     m_vPointA;
        _float3     m_vPointB;

    }GIDBOS_DESC;
protected:
    CM_Gidbos(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CM_Gidbos(const CM_Gidbos& rhs);
    virtual ~CM_Gidbos() = default;

public:
    virtual void        Enter_State(int newState);


public:
    virtual HRESULT     Initialize_Prototype(); /*원형 객체가 생성될때 부르는 Initialize*/
    virtual HRESULT     Initialize_Copytype(void* pArg); /*사본 객체가 생성될때 부르는 Initialize*/

    virtual void        Update_Priority(_float fTimeDelta);
    virtual void        Update(_float fTimeDelta);
    virtual void        Update_Late(_float fTimeDelta);
    virtual void        Update_Render(_float fTimeDelta) override;
    virtual HRESULT Render();
public:
    void                    Register_Anim();
    void                    Idle_Behavior(_float fTimeDelta);
    void                    Walk_Behavior(_float fTimeDelta);

public:
    virtual   HRESULT           Ready_PartObjects(void* pArg);
    virtual  HRESULT             Ready_Components(void* pArg);
public:
    virtual void        AIState_Change(_float fTimeDelta) override;
    virtual void        Update_Movement(_float fTimeDelta) override;
    virtual void        Render_StateDebug(int* pArg);
public:
    virtual     void    OnCollisionEnter(_uint iGroup, CCollider_Base* pOther);

public:
    static CM_Gidbos* Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;


public:
    HRESULT                         Ready_States();

private:
    class CRangeCollider* m_pRadiusTrigger=nullptr;

    bool        m_bHat = true;//위에 장애물이있을때 true없을떄 false
    _float         m_fChangeDir = 1.f;

    _vector         m_vMoveDir;     //현재 이동방향
    _vector         m_vMoveTargetPos;//실제 도착지점

    _float          m_fMoveLength = 5.f;
    _vector         m_vTargetPos;

    _vector         m_vInitPos; //초기위치, 왕복 기준점..

    _vector         m_vPointA;
    _vector         m_vPointB;


};

NS_END
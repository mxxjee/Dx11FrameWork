#pragma once
#include "CInteractionObject.h"

NS_BEGIN(Client)
class CInteraction_TriggerBox;

class CInteraction_JackyBall :
    public CInteractionObject
{
protected:
    CInteraction_JackyBall(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CInteraction_JackyBall(const CInteraction_JackyBall& rhs);
    virtual ~CInteraction_JackyBall() = default;

public:
    virtual HRESULT     Initialize_Copytype(void* pArg); /*사본 객체가 생성될때 부르는 Initialize*/
    virtual HRESULT     Ready_PartObjects(void* pArg);
    virtual void        Update(_float fTimeDelta);
    virtual void        Update_Late(_float fTimeDelta);
public:
    virtual bool IsInteratable() override;
    virtual void Enter_InteractRange() override;
    virtual void Exit_InteractRange() override;

    virtual void Enter_Interaction() override;
    virtual void Stay_Interaction(_float fTimeDelta) override;
    virtual void Exit_Interaction() override;

public:
    virtual     void    OnCollisionEnter(_uint iGroup, CCollider_Base* pOther);
    virtual void                Set_EnableCollision(bool b);
    void            Set_Dead();

public:
    void        Dead_Behavior(_float fTimeDelta);
private:
    void        Update_MonsterGrab();
public:
            //누가잡았는지 세팅하는 함수
    void        Set_Owner(CGameObject* pOwner) { m_pOwner = pOwner; }
    CGameObject* Get_Owner() { return m_pOwner; }
    virtual void        Throw();
    wstring                Print_Owner();
public:
    static CInteraction_JackyBall* Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext);
    // CGameObject을(를) 통해 상속됨
    virtual CGameObject* Clone(void* pArg) override;
    virtual     void    Free() override;

    virtual     void    PushOut(_float3 vOutPush);

public:
    virtual void        Set_InteractionMode(bool b);
    CInteraction_TriggerBox* Get_TriggerBox() { return m_pTriggerBox; }
private:
    CInteraction_TriggerBox* m_pTriggerBox = nullptr;
    class       CGameObject* m_pOwner = nullptr;

    bool            CanInteractive = true;
    _float           m_fInteractionCoolTime = 1.f;
    _float           m_fTime = 0.f;

private:
    _float          m_fDissolveSpeed = 1.f;
    _float          m_fDissolveAlpha = 0.f;

};
NS_END


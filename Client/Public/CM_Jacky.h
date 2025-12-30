#pragma once
#include "CMonster.h"

NS_BEGIN(Client)
class CInteractionObject;
class CInteraction_JackyBall;
class CPlayer;

class CM_Jacky :
    public CMonster
{
public:
    enum JackyState
    {
        ESCAPE = 10,
        LIFTING,    //들기 시작하는상태이후 iDLE로 설정
        THROW,
        END,

        ////////여기는 register anim하기위한 enum값들
        LIFTING_MOVE = 99,


        
    };

    struct JackyActionInput
    {
        bool bLift = false;
        bool bThrow = false;
        bool bEscape = false;

    };
protected:
    CM_Jacky(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CM_Jacky(const CM_Jacky& Prototype);
    virtual ~CM_Jacky() = default;


    virtual HRESULT Render();
private:
    void                    Register_Anim();
    HRESULT                 Ready_Components(void* pArg);
    HRESULT                 Ready_States();

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
    virtual void        Update_Render(_float fTimeDelta);

public:
  //  virtual void        Turn(_float fTimeDelta);

public:
    static CM_Jacky* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;

public:
    void                    Idle_Behavior(_float fTimeDelta);
    void                    Walk_Behavior(_float fTimeDelta);
    void                    Escape_Behavior(_float fTimeDelta);


    void                    Setting_Target();   // 매프레임마다 상태에 따라서 타겟세팅
    void                    Check_Escape();

public:
    void        Set_ChaseTargetObj(CInteraction_JackyBall* pObj) { m_pChaseTarget = pObj; }
public:
    bool        Is_Carrying() { return m_pInteractionObject != nullptr; }
    CInteractionObject* Get_InteractionObect() { return m_pInteractionObject; }
    bool        Is_CanMove() { return m_bCanMove; }
    
    void        Grab(CInteractionObject* pObj);
    void        Throw();

public:

public:
    CM_Jacky::JackyActionInput* Get_JackyActionInput() { return &m_JackyActionInput; }

private:
    CInteractionObject*     m_pInteractionObject = nullptr; //소유중인 인터렉션
    CInteraction_JackyBall* m_pChaseTarget = nullptr;       //항상 쫓아가기위한 포인터저장

    CPlayer*             m_pPlayer = nullptr;

public:
    virtual string Convert_String_To_Enum(_uint eState);
    virtual     void    OnCollisionEnter(_uint iGroup, CCollider_Base* pOther);

private:
    bool        m_bCanMove = false;       //true면움직임 시작, false면 움직임X



    bool        m_bRotate = false;
    _vector     m_vTargetPos;       //바라보기위한 지점, idle때만갱신
    CM_Jacky::JackyActionInput        m_JackyActionInput;
    
    _float          m_fThrowTime = 0.f;
    _float          m_fThrowTargetTime = 0.f;

};
NS_END


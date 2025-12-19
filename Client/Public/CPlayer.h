#pragma once
#include "CAnimModelObject.h"
#include "VertexData.h"
#include "Client_Defines.h"

/*움직임과 상태제어만 한다.
상태에 따른 애니메이션처리는 Body객체가 한다.*/

namespace Engine
{
    class CBody;
    class CInput_Manager;
    class CNavigation;
    class CGravity;
    class CBoxColliderComponent;


}
NS_BEGIN(Client)
class CInteractionObject;
class CPlayerState;
class CGameManager;


class CPlayer :
    public CAnimModelObject
{
public:
    enum HoldKey
    {
        HOLD_B,
        HOLD_T,
        HoldKey_End
    };
    enum class PLAYER_STATE
    {
        NONE,
        IDLE,
        RUN,
        JUMP,
        ATTACK ,
        HOLD_ATTACK,
        HOLD_SHIELD,
        SLASH_SHIELD,
        LADDER,
        PUSH,
        ITEMGET,
        CARRY,
        TALK,
        FALL,
        DAMANGE,
        END
    };

protected:
    CPlayer(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CPlayer(const CPlayer& rhs);
    virtual ~CPlayer() = default;

public:
    virtual HRESULT     Initialize_Prototype(); /*원형 객체가 생성될때 부르는 Initialize*/
    virtual HRESULT     Initialize_Copytype(void* pArg); /*사본 객체가 생성될때 부르는 Initialize*/

    virtual void        Update_Priority(_float fTimeDelta);
    virtual void        Update(_float fTimeDelta);
    virtual void        Update_Late(_float fTimeDelta);
    virtual void        Update_Render(_float fTimeDelta) override;


    virtual HRESULT Render();

private:
    void        Enter_State(int newState);
    void        Update_State(float fTimeDelta);          //바로실행해야할것들 처리

    void        Update_Input(_float fTimeDelta);
    void        Update_Fall(_float fTimeDelta);     //셀타입에 따라서 떨어짐 체크

    void        Update_HoldTime(_float fTimeDelta);
    void        Check_HoldTime(HoldKey Key,KeyCode KeyCode,_float fTimeDelta);

    void        Update_Movement(_float fTimeDelta);
         //항상 우선순위를 갖는 스테이트들 먼저 체크

    void        Ladder_Movement(_float fTimeDelta);
    void        Normal_Movement(_float fTimeDelta);
    void        Hold_Movement(_float fTimeDelta);
    
public:     
    //상태값, update돌릴 state  클래스 변경
    virtual void            Change_State(int newState);
    PLAYER_INPUT*           Get_Input() { return &m_Input; }
    ACTION_CONTROL* Get_ActionControl() { return &m_ActionControl; }

    void            Set_CanAttackEnable(bool b) { m_ActionControl.m_bCanAttack = b; }
    bool            Get_CanAttackEnable() { return m_ActionControl.m_bCanAttack; }
 
    void            Set_CanShieldEnable(bool b) { m_ActionControl.m_bCanShield = b; }
    bool            Get_CanShieldEnable() { return m_ActionControl.m_bCanShield; }


    void            Set_CanMove(bool b) { m_ActionControl.m_bCanMove = b; }
    void            Set_Hold(_int idx, bool b) { m_ActionControl.SetHold(idx, b); }
    bool            Get_Hold(_int idx) { return m_ActionControl.IsHold(idx); }

    void            Set_FixDir(bool b) { m_ActionControl.m_bFixDir = b; }
    bool            Get_FixDir() { return m_ActionControl.m_bFixDir; }

    void            Reset_ActionControl() { m_ActionControl.Reset(); }
    wstring         Get_CurrentAnimKey();

public:
    //상태에따른 메쉬 비지블
    void        Set_VisibleMesh(const wstring& MeshName, bool bVisible);
    void        Set_HideWeapons();
    void        Show_Weapons();
    void        Set_Default();

public:
    void            OnAttackBegin();
    void            OnAttackEnd();
    void            Set_ShieldEnable(bool b);

public:
    void        Respawn();
    void        Change_MainNavMesh();
public:
    static CPlayer* Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;

public:
    bool        Get_IsFront() { return m_bHitFront; }

public:
    void        Damage_Behavior();
    void        Push_Interaction_Behavior(_float fTimeDelta);
#ifdef _DEBUG
public:
    virtual void            Render_Transform_Imgui() override;
#endif

public:
    virtual HRESULT     Ready_Components(void* pArg);
    virtual HRESULT     Ready_PartObjects(void* pArg);
    HRESULT                Ready_States();

    void                Reserve_Animation_To_Body(_wstring AnimKey, bool bNextAnimLoop,bool immediately=false);

public:
    virtual     void    OnCollisionEnter(_uint iGroup, CCollider_Base* pOther);
    virtual     void    OnCollisionStay(_uint iGroup, CCollider_Base* pOther);
    virtual     void    OnCollisionExit(_uint iGroup, CCollider_Base* pOther);


private:
                    //길어질거같아서 함수로빼기
    void            Check_Interaction_Collision(CCollider_Base* pOther);

    void            Check_Interaction_Stay_Collision(CCollider_Base* pOther);

    //길어질거같아서 함수로빼기
    void            Check_Interaction_ExitCollision(CCollider_Base* pOther);

public:
    class CBoxColliderComponent* GetCollider() { return m_pCollider; }

private:
    Engine::CNavigation* m_pNavigationCom = { nullptr };
    CGravity* m_pGravity = { nullptr };
    CBoxColliderComponent* m_pCollider = { nullptr };

private:
    bool bPressed = false;
    bool m_bActionInput = false;

    CInput_Manager*         m_pInputManager = nullptr;
    int                     iHp = 5;


public:
    void                JumpMovement(_float fTimeDelta);
#ifdef _DEBUG
    virtual             string Convert_String_To_Enum(_uint eState);
    virtual void        Render_StateDebug(int* pArg);

#endif // _DEBUG

private:
            //Damage진입시 수행Push_Behavior
    void    OnDamageBehavior();
    void    UpdateFlash(_float fTimeDelta); //피격시 깜빡거림 활성화

public:
    void    Shield_Hit_Behavior();  //쉴드 가드 중 맞았을때 행동
    void    Push_Behavior();        //밀리는 동작
    bool    Set_CarryAndThrowState(CInteractionObject* pObj);
private:
    PLAYER_INPUT        m_Input;
    ACTION_CONTROL      m_ActionControl;

public:
    void        Set_DamageRender(float f) { m_ActionControl.m_fDamage = f; }
    float        Get_DamageRender() { return  m_ActionControl.m_fDamage; }
    
    void    Set_Flash(bool b);
    void    Reset_Flash() { m_bFlash = false; m_fDamageTime = 0.f; }

    DIRECTION       Get_PushDir() { return m_PushDir; }
    DIRECTION       Get_CurDir() { return m_InputDir; }
private:
    float   m_fDamageTime = 0.f;

    bool    m_bFlash = false;   //피격시 깜빡거림
    bool    m_bCanCollision = true;

    

private:
    UMap<_uint, CPlayerState*>       m_States;

    CPlayerState*       m_pCurState=nullptr;
    CPlayerState*       m_pNextState = nullptr;

private:
    float           m_fInitSpeed = 0.f;
    bool            m_bHitFront = false;


private:
    //미는것에 대한 정보.,.
    DIRECTION       m_InputDir = DIRECTION::END;
    DIRECTION      m_PushDir = DIRECTION::END;

    CInteractionObject*     m_CarryObject = nullptr;

private:
    CGameManager* m_pGameManager = nullptr;

};

NS_END
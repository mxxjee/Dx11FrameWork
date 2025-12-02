#pragma once
#include "CModelObject.h"
#include "Client_Defines.h"

namespace Engine
{
    class CNavigation;
    class CBoxColliderComponent;

}

NS_BEGIN(Client)
class CMonsterState;
class CMonster_Body;

class CMonster :
    public CModelObject
{
public:
    enum class MONSTER_BASE_STATE
    {
        NONE, INTRO,IDLE, WALK, RUN, JUMP, ATTACK, DAMAGE, DIE,END
    };
public:
    typedef struct MonsterDesc : public CModelObject::MODELOBJECT_DESC
    {
        int MaxHp;
        int iAttack;
        float fActionRange = 3.f;
        _uint   iLevelID=ENUM_TO_UINT(LEVEL_ID::TOWN);       //스폰된 레벨

    }MONSTER_DESC;
protected:
    CMonster(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CMonster(const CMonster& rhs);
    virtual ~CMonster() = default;

public:
    virtual HRESULT     Initialize_Prototype(); /*원형 객체가 생성될때 부르는 Initialize*/
    virtual HRESULT     Initialize_Copytype(void* pArg); /*사본 객체가 생성될때 부르는 Initialize*/

    virtual void        Update_Priority(_float fTimeDelta);
    virtual void        Update(_float fTimeDelta);
    virtual void        Update_Late(_float fTimeDelta);
    virtual void        Update_Render(_float fTimeDelta) override;


    virtual HRESULT Render();

public:
    virtual void    Set_Active(bool _b);
public:
    virtual void        AIState_Change(_float fTimeDelta) {};
    _wstring    Get_AnimKey(CMonster::MONSTER_BASE_STATE eType);

public:
    virtual void            Change_State(int newState);
    MONSTER_ACTION_CONTORL* Get_ActionControl() { return &m_ActionControl; }

public:
    virtual string Convert_String_To_Enum(_uint eState);

private:
    HRESULT     Ready_Resource(void* pArg);
    


public:
    bool        Is_Dead() { return m_ActionControl.m_bDead; }
public:
    virtual         void        UpdateOnIdleState() {};

private:
    virtual   HRESULT     Ready_PartObjects(void* pArg);
    virtual HRESULT     Ready_Components(void* pArg);
   
   
public:
    void                Reserve_Animation_To_Body(_wstring AnimKey, bool bNextAnimLoop);
    virtual void                Update_Movement(_float fTimeDelta) {};

public:
    /////////////////////State Behavior////////////////
    virtual void        Intro_Behavor() {};


    void                Damage_Behavior(_float fTimeDelta);

    bool                Is_InRange(_float fDistance);
public:
    static CMonster* Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;

public: 
    void        Set_Dead();



protected:
    MONSTER_ACTION_CONTORL      m_ActionControl;
    int             iHp;
    int             iMaxHp;
    int             iAttack = 10;
    float           fActionRange=0.f;
    
                    //현재 속한 레벨의 아이디
    int             m_iLevelID= ENUM_TO_UINT(LEVEL_ID::TOWN);


protected:
    float           m_fTime = 0.f;
    float            m_fDamageTime = 0.f;
protected:
    UMap<_uint, CMonsterState*>       m_States;

    CMonsterState* m_pCurState = nullptr;
    CMonsterState* m_pNextState = nullptr;

     CMonster_Body*      m_pMonsterBody = nullptr;

    MONSTER_BASE_STATE      m_eCurState = MONSTER_BASE_STATE::NONE;
    CBoxColliderComponent*  m_pCollider = { nullptr };


public:
    virtual     void    OnCollisionEnter(_uint iGroup, CCollider_Base* pOther);
    virtual     void    OnCollisionStay(_uint iGroup, CCollider_Base* pOther);
    virtual     void    OnCollisionExit(_uint iGroup, CCollider_Base* pOther);


public:
    bool            Is_CanMove() { return m_bCanMove; }
    void            Set_CanMove(bool b) { m_bCanMove = b; }

protected:
    float           m_fInitSpeed = 0.f;
    bool            m_bCanMove = true;

protected:
    Engine::CNavigation* m_pNavigationCom = { nullptr };


};

NS_END
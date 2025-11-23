#pragma once
#include "CModelObject.h"
#include "VertexData.h"
#include "Client_Defines.h"

/*움직임과 상태제어만 한다.
상태에 따른 애니메이션처리는 Body객체가 한다.*/

namespace Engine
{
    class CBody;
    class CInput_Manager;
    class CNavigation;

}
NS_BEGIN(Client)
class CPlayerState;

class CPlayer :
    public CModelObject
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
    void        Update_Input(_float fTimeDelta);
    void        Update_Fall(_float fTimeDelta);     //셀타입에 따라서 떨어짐 체크

    void        Update_HoldTime(_float fTimeDelta);
    void        Check_HoldTime(HoldKey Key,KeyCode KeyCode,_float fTimeDelta);

    void        Update_Movement(_float fTimeDelta);

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


public:
    //상태에따른 메쉬 비지블
    void        Set_VisibleMesh(const wstring& MeshName, bool bVisible);
    void        Set_HideWeapons();
    void        Show_Weapons();
    void        Set_Default();

public:
    void        Respawn();
public:
    static CPlayer* Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;

public:
    virtual HRESULT     Ready_Components(void* pArg);
    virtual HRESULT     Ready_PartObjects(void* pArg);
    HRESULT                Ready_States();
    void                Reserve_Animation_To_Body(_wstring AnimKey, bool bNextAnimLoop);


private:
    CNavigation* m_pNavigationCom = { nullptr };
private:
    bool bPressed = false;
    bool m_bActionInput = false;

    CInput_Manager*         m_pInputManager = nullptr;
    int                     iHp = 5;


public:
    virtual string Convert_String_To_Enum(_uint eState);


private:
    PLAYER_INPUT        m_Input;
    ACTION_CONTROL      m_ActionControl;



private:
    UMap<_uint, CPlayerState*>       m_States;

    CPlayerState*       m_pCurState=nullptr;
    CPlayerState*       m_pNextState = nullptr;

private:
    float           m_fInitSpeed = 0.f;
};

NS_END
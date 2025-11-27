#pragma once
#include "CMonster.h"

namespace Engine
{
    class CGravity;
}
NS_BEGIN(Client)
class CM_GreenZol :
    public CMonster
{
    enum class GreenZolState
    {
        HIDE=10,END
    };

protected:
    CM_GreenZol(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CM_GreenZol(const CM_GreenZol& rhs);
    virtual ~CM_GreenZol() = default;


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
    virtual void            Intro_Behavor();
    virtual void            Change_State(int newState);

public:
    HRESULT          Ready_Component(void* pArg);
public:
    virtual void        AIState_Change(_float fTimeDelta) override;
    virtual void        Update_Movement(_float fTimeDelta) override;
    virtual void        Render_StateDebug(int* pArg);
private:
    void                Move_RandomDir();
public:
    virtual         void        UpdateOnIdleState() override;
    void                        Jump_To_Player(_float fTimeDelta);

public:
    static CM_GreenZol* Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;

public:
    HRESULT                Ready_States();
   

public:
    void            Idle_Behavior(_float fTimeDelta);
    void            JumpStart_Behavior();
    bool            Get_IsHide() { return m_bHide; }
    bool            Get_IsOnGround();
private:
    MONSTER_BASE_STATE  m_eCurState;
    float               m_fSecond = 1.f;
    
private:
    float           m_fSpawnRange = 3.f;        //스폰하기위한 범위
    bool             m_bHide = false;
    bool             m_bJump = false;

private:
    CGravity* m_pGravity = { nullptr };
};
NS_END


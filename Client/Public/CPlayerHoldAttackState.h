#pragma once
#include "CPlayerState.h"

NS_BEGIN(Client)
class CPlayer;

class CPlayerHoldAttackState :
    public CPlayerState
{
    enum class NextAnim{NONE,SLASH_SHIELD};
    enum class Phase {None,Start,Loop,End};

private:
    CPlayerHoldAttackState();
    virtual ~CPlayerHoldAttackState();
public:
    virtual void        Enter(CPlayer* pPlayer)override;
    virtual bool        Update(CPlayer* pPlayer, _float fTimeDelta)override;
    virtual void        Update_Late(CPlayer* pPlayer, _float fTimeDelta)override;
    virtual void        Exit(CPlayer* pPlayer)override;

private:
    void            ChangePhase(CPlayer* pPlayer);      //페이즈만 변경
    void            ChangeState(CPlayer* pPlayer);//아예 다른상태로 전이

private:
    void            OnChargeCompleteEnter(CPlayer* pPlayer);    //특정시간이후에 차징완료 설정


private:
    void        Hold_Movement(CPlayer* pPlayer);
public:
    static CPlayerHoldAttackState* Create();

private:
    Phase   m_ePhase = Phase::None;
    NextAnim    m_eNextAnim = NextAnim::NONE;

    bool        m_bChange = false;
    bool        m_bChangeState = false;

    _float      m_fTime = 0.f;
    _float      m_fChargeEndTime = 1.f;

};

NS_END
#pragma once
#include "CPlayerState.h"

NS_BEGIN(Client)
class CPlayer;

class CPlayerHoldAttackState :
    public CPlayerState
{
    enum class Phase {None,Start,Loop,End};

private:
    CPlayerHoldAttackState();
    virtual ~CPlayerHoldAttackState();
public:
    virtual void        Enter(CPlayer* pPlayer)override;
    virtual void        Update(CPlayer* pPlayer)override;
    virtual void        Update_Late(CPlayer* pPlayer)override;
    virtual void        Exit(CPlayer* pPlayer)override;

private:
    void        Hold_Movement(CPlayer* pPlayer);
public:
    static CPlayerHoldAttackState* Create();

private:
    Phase   m_ePhase = Phase::None;
    bool        m_bChange = false;
};

NS_END
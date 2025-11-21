#pragma once
#include "CPlayerState.h"
NS_BEGIN(Client)
class CPlayer;

class CPlayerHoldShield :
    public CPlayerState
{
    enum class Phase { None, Start, Loop, End };

private:
    CPlayerHoldShield();
    virtual ~CPlayerHoldShield();
public:
    virtual void        Enter(CPlayer* pPlayer)override;
    virtual void        Update(CPlayer* pPlayer)override;
    virtual void        Update_Late(CPlayer* pPlayer)override;
    virtual void        Exit(CPlayer* pPlayer)override;

public:
    static CPlayerHoldShield* Create();
private:
    Phase   m_ePhase = Phase::None;
    bool        m_bChange = false;

};
NS_END

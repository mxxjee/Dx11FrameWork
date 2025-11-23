#pragma once
#include "CPlayerState.h"

NS_BEGIN(Client)

class CPlayerTalkState :
    public CPlayerState
{
    enum class Phase { None, Start, Loop, End };


private:
    CPlayerTalkState();
    virtual ~CPlayerTalkState();

public:
    virtual void        Enter(CPlayer* pPlayer)override;
    virtual void        Update(CPlayer* pPlayer, _float fTimeDelta)override;
    virtual void        Update_Late(CPlayer* pPlayer, _float fTimeDelta)override;
    virtual void        Exit(CPlayer* pPlayer)override;


public:
    static CPlayerTalkState* Create();

private:
    Phase   m_ePhase = Phase::None;

    bool        m_bChange = false;
    bool        m_bChangeState = false;

};

NS_END
#pragma once
#include "CPlayerState.h"

NS_BEGIN(Client)
class CPlayer;
class CPlayerAttackState :
    public CPlayerState
{
    enum class NextAnim{NONE,IDLE,HOLD,SHIELD,SLASH_SHIELD};
private:
    CPlayerAttackState();
    virtual ~CPlayerAttackState();


public:
    virtual void        Enter(CPlayer* pPlayer)override;
    virtual void        Update(CPlayer* pPlayer)override;
    virtual void        Update_Late(CPlayer* pPlayer)override;
    virtual void        Exit(CPlayer* pPlayer)override;

public:
    static CPlayerAttackState* Create();
    
private:
    NextAnim e_NextAnim = NextAnim::NONE;
    bool        m_bChange = false;

};

NS_END
#pragma once
#include "CPlayerState.h"

NS_BEGIN(Client)
class CPlayer;
class CPlayerAttackState :
    public CPlayerState
{
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
    bool        m_bChange = false;
};

NS_END
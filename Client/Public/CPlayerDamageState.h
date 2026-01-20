#pragma once
#include "CPlayerState.h"

NS_BEGIN(Client)
class CPlayer;
class CPlayerDamageState :
    public CPlayerState
{
private:
    CPlayerDamageState();
    virtual ~CPlayerDamageState();


public:
    virtual void        Enter(CPlayer* pPlayer)override;
    virtual bool        Update(CPlayer* pPlayer, _float fTimeDelta)override;
    virtual void        Update_Late(CPlayer* pPlayer, _float fTimeDelta)override;
    virtual void        Exit(CPlayer* pPlayer)override;


public:
    static CPlayerDamageState* Create();

};

NS_END
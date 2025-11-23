#pragma once
#include "CPlayerState.h"

NS_BEGIN(Client)
class CPlayer;

class CPlayerRunState :
    public CPlayerState
{
private:
    CPlayerRunState();
    virtual ~CPlayerRunState();


public:
    virtual void        Enter(CPlayer* pPlayer)override;
    virtual void        Update(CPlayer* pPlayer, _float fTimeDelta)override;
    virtual void        Exit(CPlayer* pPlayer)override;

public:
    static CPlayerRunState* Create();
};
NS_END
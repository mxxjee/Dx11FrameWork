#pragma once
#include "CPlayerState.h"

NS_BEGIN(Client)
class CPlayer;

class CPlayerIdleState final:
    public CPlayerState
{
private:
    CPlayerIdleState();
    virtual ~CPlayerIdleState();


public:
    void        Enter(CPlayer* pPlayer);
    void        Update(CPlayer* pPlayer);
    void        Exit(CPlayer* pPlayer);



};
NS_END


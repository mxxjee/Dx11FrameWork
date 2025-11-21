#pragma once
#include "CPlayerState.h"
NS_BEGIN(Client)
class CPlayer;

class CPlayerSlashShieldState :
    public CPlayerState
{
    //전이 될 수 잇는 다음 애니메이션들
    enum class NextAnim { NONE, ATTACK,SHIELD };

    //쉴드내애서의 ST,LP,ED
    enum class Phase { None, Loop, End };

private:
    CPlayerSlashShieldState();
    virtual ~CPlayerSlashShieldState();

public:
    virtual void        Enter(CPlayer* pPlayer)override;
    virtual void        Update(CPlayer* pPlayer)override;
    virtual void        Update_Late(CPlayer* pPlayer)override;
    virtual void        Exit(CPlayer* pPlayer)override;

private:

    void        Hold_Movement(CPlayer* pPlayer);
public:
    static CPlayerSlashShieldState* Create();

};
NS_END

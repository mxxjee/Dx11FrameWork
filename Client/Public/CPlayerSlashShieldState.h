#pragma once
#include "CPlayerState.h"
NS_BEGIN(Client)
class CPlayer;

class CPlayerSlashShieldState :
    public CPlayerState
{
    //전이 될 수 잇는 다음 애니메이션들
    enum class NextAnim { NONE, IDLE,ATTACK,SHIELD,ATTACK_HOLD,ATTACK_END };

    //ST,LP,ED
    enum class Phase { None, Loop, End };

private:
    CPlayerSlashShieldState();
    virtual ~CPlayerSlashShieldState();

public:
    virtual void        Enter(CPlayer* pPlayer)override;
    virtual bool        Update(CPlayer* pPlayer, _float fTimeDelta)override;
    virtual void        Update_Late(CPlayer* pPlayer, _float fTimeDelta)override;
    virtual void        Exit(CPlayer* pPlayer)override;


private:
    void        Hold_Movement(CPlayer* pPlayer);
    void        Change_Other_State(CPlayer* pPlayer);
    void        Change_Phase(CPlayer* pPlayer);
public:
    static CPlayerSlashShieldState* Create();

private:
    NextAnim e_NextAnim = NextAnim::NONE;
    Phase   m_ePhase = Phase::None;

private:
    bool        m_bChangeState = false; //state바꾸기

   
};
NS_END

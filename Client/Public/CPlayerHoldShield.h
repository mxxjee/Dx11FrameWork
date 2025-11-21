#pragma once
#include "CPlayerState.h"
NS_BEGIN(Client)
class CPlayer;

class CPlayerHoldShield :
    public CPlayerState
{
    //전이 될 수 잇는 다음 애니메이션들
    enum class NextAnim { NONE, IDLE,RUN, ATTACK,SLASH_SHIELD };

                //쉴드내애서의 ST,LP,ED
    enum class Phase { None, Start, Loop, End };

private:
    CPlayerHoldShield();
    virtual ~CPlayerHoldShield();
public:
    virtual void        Enter(CPlayer* pPlayer)override;
    virtual void        Update(CPlayer* pPlayer)override;
    virtual void        Update_Late(CPlayer* pPlayer)override;
    virtual void        Exit(CPlayer* pPlayer)override;

private:
    void        Change_Other_State(CPlayer* pPlayer);
    void        Change_Phase(CPlayer* pPlayer);

    void        Hold_Movement(CPlayer* pPlayer);
public:
    static CPlayerHoldShield* Create();

private:
    Phase   m_ePhase = Phase::None;
    NextAnim e_NextAnim = NextAnim::NONE;

    bool        m_bChangePhase = false;  //pahse 바꾸기
    bool        m_bChangeState = false; //state바꾸기


    bool        m_bExitState = false;

};
NS_END

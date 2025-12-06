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
    enum class Phase { None, Start, Loop, Hit,End };

private:
    CPlayerHoldShield();
    virtual ~CPlayerHoldShield();
public:
    virtual void        Enter(CPlayer* pPlayer)override;
    virtual void        Update(CPlayer* pPlayer, _float fTimeDelta)override;
    virtual void        Update_Late(CPlayer* pPlayer, _float fTimeDelta)override;
    virtual void        Exit(CPlayer* pPlayer)override;

public:
                //쉴드 가드 성공했따면. 실행할애니메이션
    void        Hit_Shield(CPlayer* pPlayer);
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

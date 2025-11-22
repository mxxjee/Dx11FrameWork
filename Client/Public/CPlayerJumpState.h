#pragma once
#include "CPlayerState.h"

NS_BEGIN(Client)
class CPlayer;

class CPlayerJumpState:
	public CPlayerState
{
	enum class NextAnim { NONE, IDLE, JUMP_SLASH};
	enum class Phase { None, Start,  End };
private:
    CPlayerJumpState();
    virtual ~CPlayerJumpState();
public:
    virtual void        Enter(CPlayer* pPlayer)override;
    virtual void        Update(CPlayer* pPlayer)override;
    virtual void        Update_Late(CPlayer* pPlayer)override;
    virtual void        Exit(CPlayer* pPlayer)override;

private:
    void            ChangePhase(CPlayer* pPlayer);      //페이즈만 변경
    void            ChangeState(CPlayer* pPlayer);//아예 다른상태로 전이


public:
    static CPlayerJumpState* Create();

private:
    Phase   m_ePhase = Phase::None;
    NextAnim    m_eNextAnim = NextAnim::NONE;

    bool        m_bChange = false;
    bool        m_bChangeState = false;


};

NS_END
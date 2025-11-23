#pragma once
#include "CPlayerState.h"

NS_BEGIN(Client)

class CPlayerLadderState :
    public CPlayerState
{
    enum class NextAnim { NONE, IDLE};
    enum class Phase { None, Start, Loop,End };
private:
    CPlayerLadderState();
    virtual ~CPlayerLadderState();
public:
    virtual void        Enter(CPlayer* pPlayer)override;
    virtual void        Update(CPlayer* pPlayer, _float fTimeDelta)override;
    virtual void        Update_Late(CPlayer* pPlayer, _float fTimeDelta)override;
    virtual void        Exit(CPlayer* pPlayer)override;

private:
    void        Hold_Movement(CPlayer* pPlayer);
    void            ChangePhase(CPlayer* pPlayer);      //페이즈만 변경
    void            ChangeState(CPlayer* pPlayer);//아예 다른상태로 전이


public:
    static CPlayerLadderState* Create();

private:
    Phase   m_ePhase = Phase::None;
    NextAnim    m_eNextAnim = NextAnim::NONE;

    bool        m_bChange = false;
    bool        m_bChangeState = false;
};
NS_END


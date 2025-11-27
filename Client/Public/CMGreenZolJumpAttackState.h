#pragma once
#include "CMonsterState.h"


NS_BEGIN(Client)
class CMonster;
class CM_GreenZol;

class CMGreenZolJumpAttackState :
    public CMonsterState
{
    enum PHASE {SIGN,START,LOOP,ED,END};
private:
    CMGreenZolJumpAttackState();
    virtual ~CMGreenZolJumpAttackState();

public:
    virtual void        Enter(CMonster* pMonster)override;
    virtual void        Update(CMonster* pMonster, _float fTimeDelta)override;
    virtual void        Update_Late(CMonster* pMonster, _float fTimeDelta)override;
    virtual void        Exit(CMonster* pMonster)override;

public:
    static CMGreenZolJumpAttackState* Create() {
        return new CMGreenZolJumpAttackState;
    }

private:
    CM_GreenZol*    m_pGreenZol;
    PHASE       m_ePhase = PHASE::END;
    _float          m_fTime = 0.f;
    bool            m_bJump = false;

};

NS_END
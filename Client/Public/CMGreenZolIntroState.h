#pragma once
#include "CMonsterState.h"

NS_BEGIN(Client)
class CMonster;
class CM_GreenZol;

class CMGreenZolIntroState :
    public CMonsterState
{
    enum State{WAIT,SPAWN,END};
private:
    CMGreenZolIntroState();
    virtual ~CMGreenZolIntroState();

public:
    virtual void        Enter(CMonster* pMonster)override;
    virtual void        Update(CMonster* pMonster, _float fTimeDelta)override;
    virtual void        Update_Late(CMonster* pMonster, _float fTimeDelta)override;
    virtual void        Exit(CMonster* pMonster)override;


public:
    static CMGreenZolIntroState* Create() {
        return new CMGreenZolIntroState;
    }


private:
    CM_GreenZol*        m_pGreenZol;
    State               m_eState;
};
NS_END

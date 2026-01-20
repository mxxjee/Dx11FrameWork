#pragma once
#include "CMonsterState.h"

NS_BEGIN(Client)
class CMonster;
class CM_MoriblinSword;


class CMMoriblinSwordIdleState :
    public CMonsterState
{

private:
    CMMoriblinSwordIdleState();
    virtual ~CMMoriblinSwordIdleState();

public:
    virtual void        Enter(CMonster* pMonster)override;
    virtual void        Update(CMonster* pMonster, _float fTimeDelta)override;
    virtual void        Update_Late(CMonster* pMonster, _float fTimeDelta)override;
    virtual void        Exit(CMonster* pMonster)override;


protected:
            //항상 체크해야하는 스테이트들(우선이 되는 state들 체크)
    virtual void        Check_Always(CMonster* pMonster, _float fTimeDelta);


public:
    static CMMoriblinSwordIdleState* Create() {
        return new CMMoriblinSwordIdleState;
    }
private:
    CM_MoriblinSword* m_pMoriblinSword;
};

NS_END
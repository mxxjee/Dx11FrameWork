#pragma once
#include "CMonsterState.h"

NS_BEGIN(Client)
class CMonster;

class CMonsterIdleState :
    public CMonsterState
{
private:
    CMonsterIdleState();
    virtual ~CMonsterIdleState();

public:
    virtual void        Enter(CMonster* pMonster)override;
    virtual void        Update(CMonster* pMonster, _float fTimeDelta)override;
    virtual void        Update_Late(CMonster* pMonster, _float fTimeDelta)override;
    virtual void        Exit(CMonster* pMonster)override;


public:
    static CMonsterIdleState* Create() {
        return new CMonsterIdleState;
    }


};

NS_END
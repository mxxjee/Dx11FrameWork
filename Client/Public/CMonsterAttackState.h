#pragma once
#include "CMonsterState.h"

NS_BEGIN(Client)
class CMonster;

class CMonsterAttackState :
    public CMonsterState
{
private:
    CMonsterAttackState();
    virtual ~CMonsterAttackState();

public:
    virtual void        Enter(CMonster* pMonster)override;
    virtual void        Update(CMonster* pMonster, _float fTimeDelta)override;
    virtual void        Update_Late(CMonster* pMonster, _float fTimeDelta)override;
    virtual void        Exit(CMonster* pMonster)override;


public:
    static CMonsterAttackState* Create() {
        return new CMonsterAttackState;
    }


};

NS_END
#pragma once
#include "CMonsterState.h"

NS_BEGIN(Client)
class CMonster;

class CMonsterDamageState :
    public CMonsterState
{
private:
    CMonsterDamageState();
    virtual ~CMonsterDamageState();

public:
    virtual void        Enter(CMonster* pMonster)override;
    virtual void        Update(CMonster* pMonster, _float fTimeDelta)override;
    virtual void        Update_Late(CMonster* pMonster, _float fTimeDelta)override;
    virtual void        Exit(CMonster* pMonster)override;


public:
    static CMonsterDamageState* Create() {
        return new CMonsterDamageState;
    }
};

NS_END
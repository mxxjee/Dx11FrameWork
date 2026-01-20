#pragma once
#include "CMonsterState.h"

NS_BEGIN(Client)
class CMonster;


class CMonsterDeadState :
    public CMonsterState
{
private:
    CMonsterDeadState();
    virtual ~CMonsterDeadState();

public:
    virtual void        Enter(CMonster* pMonster)override;
    virtual void        Update(CMonster* pMonster, _float fTimeDelta)override;
    virtual void        Update_Late(CMonster* pMonster, _float fTimeDelta)override;
    virtual void        Exit(CMonster* pMonster)override;


public:
    static CMonsterDeadState* Create() {
        return new CMonsterDeadState;
    }

};
NS_END

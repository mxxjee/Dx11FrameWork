#pragma once
#include "CMonsterState.h"

NS_BEGIN(Client)
class CMonster;

class CMonsterWalkState :
    public CMonsterState
{
private:
    CMonsterWalkState();
    virtual ~CMonsterWalkState();

public:
    virtual void        Enter(CMonster* pMonster)override;
    virtual void        Update(CMonster* pMonster, _float fTimeDelta)override;
    virtual void        Update_Late(CMonster* pMonster, _float fTimeDelta)override;
    virtual void        Exit(CMonster* pMonster)override;


public:
    static CMonsterWalkState* Create() {
        return new CMonsterWalkState;
    }

};

NS_END
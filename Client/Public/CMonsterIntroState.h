#pragma once
#include "CMonsterState.h"

NS_BEGIN(Client)
class CMonster;

class CMonsterIntroState :
    public CMonsterState
{
private:
    CMonsterIntroState();
    virtual ~CMonsterIntroState();

public:
    virtual void        Enter(CMonster* pMonster)override;
    virtual void        Update(CMonster* pMonster, _float fTimeDelta)override;
    virtual void        Update_Late(CMonster* pMonster, _float fTimeDelta)override;
    virtual void        Exit(CMonster* pMonster)override;


public:
    static CMonsterIntroState* Create() {
        return new CMonsterIntroState;
    }

};

NS_END
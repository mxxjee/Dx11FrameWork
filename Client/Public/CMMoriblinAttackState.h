#pragma once
#include "CMonsterState.h"

NS_BEGIN(Client)
class CMonster;
class CM_MoriblinSword;
class CMMoriblinAttackState :
    public CMonsterState
{
private:
    CMMoriblinAttackState();
    virtual ~CMMoriblinAttackState();

public:
    virtual void        Enter(CMonster* pMonster)override;
    virtual void        Update(CMonster* pMonster, _float fTimeDelta)override;
    virtual void        Update_Late(CMonster* pMonster, _float fTimeDelta)override;
    virtual void        Exit(CMonster* pMonster)override;


public:
    static CMMoriblinAttackState* Create() {
        return new CMMoriblinAttackState;
    }

private:
    virtual void        Check_Always(CMonster* pMonster, _float fTimeDelta);

private:
    CM_MoriblinSword* m_pMoriblinSword;
};

NS_END
#pragma once
#include "CMonsterState.h"

NS_BEGIN(Client)
class CMonster;
class CM_MoriblinSword;

class CMMoriblinSwordDetectState :
    public CMonsterState
{
private:
    CMMoriblinSwordDetectState();
    virtual ~CMMoriblinSwordDetectState();

public:
    virtual void        Enter(CMonster* pMonster)override;
    virtual void        Update(CMonster* pMonster, _float fTimeDelta)override;
    virtual void        Update_Late(CMonster* pMonster, _float fTimeDelta)override;
    virtual void        Exit(CMonster* pMonster)override;


public:
    static CMMoriblinSwordDetectState* Create() {
        return new CMMoriblinSwordDetectState;
    }
private:
    CM_MoriblinSword* m_pMoriblinSword;
};
NS_END


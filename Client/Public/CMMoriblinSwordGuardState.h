#pragma once
#include "CMonsterState.h"

NS_BEGIN(Client)
class CMonster;
class CM_MoriblinSword;


class CMMoriblinSwordGuardState :
    public CMonsterState
{
private:
    enum GuardType{SHIELD,SWORD,END};
private:
    CMMoriblinSwordGuardState();
    virtual ~CMMoriblinSwordGuardState();

public:
    virtual void        Enter(CMonster* pMonster)override;
    virtual void        Update(CMonster* pMonster, _float fTimeDelta)override;
    virtual void        Update_Late(CMonster* pMonster, _float fTimeDelta)override;
    virtual void        Exit(CMonster* pMonster)override;


protected:
    //항상 체크해야하는 스테이트들(우선이 되는 state들 체크)
    virtual void        Check_Always(CMonster* pMonster, _float fTimeDelta);


public:
    static CMMoriblinSwordGuardState* Create() {
        return new CMMoriblinSwordGuardState;
    }
private:
    CM_MoriblinSword* m_pMoriblinSword;

private:
    GuardType   m_eType;
    float m_fTime = 0.f;
};

NS_END
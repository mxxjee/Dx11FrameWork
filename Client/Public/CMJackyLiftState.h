#pragma once
#include "CMonsterState.h"

NS_BEGIN(Client)
class CMonster;
class CM_Jacky;
class CMJackyLiftState :
    public CMonsterState
{

private:
    CMJackyLiftState();
    virtual ~CMJackyLiftState();


public:
    virtual void        Enter(CMonster* pMonster)override;
    virtual void        Update(CMonster* pMonster, _float fTimeDelta)override;
    virtual void        Update_Late(CMonster* pMonster, _float fTimeDelta)override;
    virtual void        Exit(CMonster* pMonster)override;


protected:
    //항상 체크해야하는 스테이트들(우선이 되는 state들 체크)
    virtual void        Check_Always(CMonster* pMonster, _float fTimeDelta);


public:
    static CMJackyLiftState* Create() {
        return new CMJackyLiftState;
    }
private:
    CM_Jacky* m_pJacky = nullptr;

};

NS_END
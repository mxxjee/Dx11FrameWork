#pragma once
#include "CMonsterState.h"
NS_BEGIN(Client)
class CMonster;
class CM_Jacky;

class CMJackyRunState :
    public CMonsterState
{
private:
    CMJackyRunState();
    virtual ~CMJackyRunState();


public:
    virtual void        Enter(CMonster* pMonster)override;
    virtual void        Update(CMonster* pMonster, _float fTimeDelta)override;
    virtual void        Update_Late(CMonster* pMonster, _float fTimeDelta)override;
    virtual void        Exit(CMonster* pMonster)override;


protected:
    //항상 체크해야하는 스테이트들(우선이 되는 state들 체크)
    virtual void        Check_Always(CMonster* pMonster, _float fTimeDelta);


public:
    static CMJackyRunState* Create() {
        return new CMJackyRunState;
    }
private:
    CM_Jacky* m_pJacky = nullptr;
    _float      m_fTime = 0.f;

};
NS_END
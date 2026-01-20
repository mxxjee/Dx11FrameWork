#pragma once
#include "CMonsterState.h"

NS_BEGIN(Client)
class CMonster;
class CM_GreenZol;

class CMGreenZolHideState :
    public CMonsterState
{
private:
    CMGreenZolHideState();
    virtual ~CMGreenZolHideState();

public:
    virtual void        Enter(CMonster* pMonster)override;
    virtual void        Update(CMonster* pMonster, _float fTimeDelta)override;
    virtual void        Update_Late(CMonster* pMonster, _float fTimeDelta)override;
    virtual void        Exit(CMonster* pMonster)override;


public:
    static CMGreenZolHideState* Create() {
        return new CMGreenZolHideState;
    }
private:
    CM_GreenZol* m_pGreenZol;
};
NS_END

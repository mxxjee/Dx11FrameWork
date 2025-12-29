#pragma once
#include "CMonsterState.h"

NS_BEGIN(Client)
class CMonster;

class CMonsterDamageState :
    public CMonsterState
{
public:
    enum class EXIT_TYPE {ANIM_END, TIMER,END};
private:
    CMonsterDamageState();
    virtual ~CMonsterDamageState();

public:
    virtual void        Enter(CMonster* pMonster)override;
    virtual void        Update(CMonster* pMonster, _float fTimeDelta)override;
    virtual void        Update_Late(CMonster* pMonster, _float fTimeDelta)override;
    virtual void        Exit(CMonster* pMonster)override;


public:
    static CMonsterDamageState* Create(EXIT_TYPE eType=EXIT_TYPE::ANIM_END) {

        CMonsterDamageState* pInstance = new CMonsterDamageState;
        if (pInstance)
            pInstance->m_eExitType = eType;

        return pInstance;
    }

    void        Set_ExitType(EXIT_TYPE eType) { m_eExitType = eType; }

private:
    EXIT_TYPE       m_eExitType = EXIT_TYPE::END;

    _float          m_fTime = 0.f;
    _float          m_fExitTime = 0.5f;
    
};

NS_END
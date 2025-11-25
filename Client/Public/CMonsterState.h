#pragma once
#include "CBase.h"
#include "Client_Defines.h"
NS_BEGIN(Engine)
class CGameInstance;
NS_END

NS_BEGIN(Client)
class CMonster;

class CMonsterState :
    public CBase
{
protected:
    CMonsterState();
    virtual ~CMonsterState() = default;

public:
    virtual void        Enter(CMonster* pMonster);
    virtual void        Update(CMonster* pMonster, _float fTimeDelta);
    virtual void        Update_Late(CMonster* pMonster, _float fTimeDelta) {};
    virtual void        Exit(CMonster* pMonster) {};


    virtual void Free();
    bool CanExit() { return m_bCanExit; }

protected:
    bool        m_bCanExit = true;        //강제 전이 가능여부



protected:
    CGameInstance* m_pGameInstance = nullptr;
    MONSTER_ACTION_CONTORL* m_pActionControl = nullptr;


};
NS_END

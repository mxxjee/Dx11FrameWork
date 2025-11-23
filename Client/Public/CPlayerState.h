#pragma once
#include "CBase.h"
#include "Client_Defines.h"


NS_BEGIN(Client)
class CPlayer;

class CPlayerState:
    public CBase
{

protected:
    CPlayerState();
    virtual ~CPlayerState() = default;



public:
    virtual void        Enter(CPlayer* pPlayer) {};
    virtual void        Update(CPlayer* pPlayer,_float fTimeDelta){};
    virtual void        Update_Late(CPlayer* pPlayer,_float fTimeDelta) {};
    virtual void        Exit(CPlayer* pPlayer){};


    virtual void Free();
    bool CanExit() { return m_bCanExit; }

protected:
    bool        m_bCanExit =true;        //강제 전이 가능여부
    PLAYER_INPUT*           pPlayerInput = nullptr;
    ACTION_CONTROL*         pActionControl = nullptr;
;
};

NS_END


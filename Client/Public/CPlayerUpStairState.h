#pragma once
#include "CPlayerState.h"

NS_BEGIN(Client)
class CPlayerUpStairState :
    public CPlayerState
{
private:
    CPlayerUpStairState();
    virtual ~CPlayerUpStairState();

public:
    virtual void        Enter(CPlayer* pPlayer)override;
    virtual bool        Update(CPlayer* pPlayer, _float fTimeDelta)override;
    virtual void        Update_Late(CPlayer* pPlayer, _float fTimeDelta)override;
    virtual void        Exit(CPlayer* pPlayer)override;

public:
    static CPlayerUpStairState* Create();

};
NS_END


#pragma once
#include "CPlayerState.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END


NS_BEGIN(Client)

class CPlayerPratFallState :
    public CPlayerState
{
    enum PHASE {LP,END};
private:
    CPlayerPratFallState();
    virtual ~CPlayerPratFallState();
public:
    virtual void        Enter(CPlayer* pPlayer)override;
    virtual bool        Update(CPlayer* pPlayer, _float fTimeDelta)override;
    virtual void        Update_Late(CPlayer* pPlayer, _float fTimeDelta)override;
    virtual void        Exit(CPlayer* pPlayer)override;

public:
    static CPlayerPratFallState* Create();
    PHASE m_CurPhase;

};

NS_END
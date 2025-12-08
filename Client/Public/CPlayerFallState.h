#pragma once
#include "CPlayerState.h"

NS_BEGIN(Engine)
class CGameInstance;
NS_END


NS_BEGIN(Client)


class CPlayerFallState :
    public CPlayerState
{
private:
    CPlayerFallState();
    virtual ~CPlayerFallState();
public:
    virtual void        Enter(CPlayer* pPlayer)override;
    virtual bool        Update(CPlayer* pPlayer, _float fTimeDelta)override;
    virtual void        Update_Late(CPlayer* pPlayer, _float fTimeDelta)override;
    virtual void        Exit(CPlayer* pPlayer)override;


public:
    static CPlayerFallState* Create();

private:                                     
    bool        m_bChange = false;
    bool        m_bChangeState = false;

private:
    float       m_fTime = 0.f;

};

NS_END
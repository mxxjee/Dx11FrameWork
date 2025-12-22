#pragma once
#include "CPlayerState.h"

namespace Engine
{
    class CMainCamera;

}
NS_BEGIN(Client)
class CPlayerStateGetItem :
    public CPlayerState
{
    enum class NextAnim { NONE, IDLE };
    enum class Phase { None, Start, Loop, End };

private:
    CPlayerStateGetItem();
    virtual ~CPlayerStateGetItem();
    

public:
    virtual void        Enter(CPlayer* pPlayer)override;
    virtual bool        Update(CPlayer* pPlayer, _float fTimeDelta)override;
    virtual void        Update_Late(CPlayer* pPlayer, _float fTimeDelta)override;
    virtual void        Exit(CPlayer* pPlayer)override;

private:
    void            ChangePhase(CPlayer* pPlayer);      //페이즈만 변경
    void            ChangeState(CPlayer* pPlayer);//아예 다른상태로 전이

private:
    void        Change_CameraState(float _fTimeDelta);


public:
    static CPlayerStateGetItem* Create();


private:
    Phase   m_ePhase = Phase::None;
    NextAnim    m_eNextAnim = NextAnim::NONE;

    bool        m_bChange = false;
    bool        m_bChangeState = false;

    _float      m_fTime = 0.f;

    bool        m_bStartAnim = false;
    bool        m_bShowUI = false;      //true이면 이벤트발생x
    CMainCamera* m_pMainCamera = nullptr;

private:
    ItemType        m_eGetItemType;

};
NS_END

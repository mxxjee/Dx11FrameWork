#pragma once
#include "CNPC.h"
#include "Client_Defines.h"

NS_BEGIN(Client)
class CNPC_KidRed :
    public CNPC
{
public:
    enum class State
    {
        WAIT, WALK, TALK, TELEPORT, END
    };


protected:
    CNPC_KidRed(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CNPC_KidRed(const CNPC_KidRed& rhs);
    virtual ~CNPC_KidRed() = default;

public:
    HRESULT     Initialize_Prototype(void* pArg); /*NPC는 clone으로 생성하지않는다.*/

    virtual void        Update_Priority(_float fTimeDelta);
    virtual void        Update(_float fTimeDelta);
    virtual void        Update_Late(_float fTimeDelta);
    virtual void        Update_Render(_float fTimeDelta) override;


    virtual HRESULT Render();

private:
    void        Walk_StartFirstChapter(_float fTimeDelta);
    void        Walk_EndFirstChapter(_float fTimeDelta);

    void        Wait_EndFirstChapter(_float fTimeDelta);
public:
    ///범위감지용///
    virtual void        Enter_InteractRange();

    virtual void    Exit_Interaction();
public:
    void        Set_StartEvent(bool b, CNPC_KidRed::State iState) { CheckTrue(m_bStartEvent); m_bStartEvent = b; m_iState = iState; }

public:
    CNavigation* Get_Navigation() { return m_pNavigationCom; }
public:
    void        Enter_State(int newState);

    void        Update_State(_float fTimeDelta);

public:
    virtual void    Enter_Interaction();
    virtual void    Stay_Interaction(_float fTimeDelta);
public:
    static CNPC_KidRed* Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContex, void* pArg);
    virtual void Free() override;

public:
    void        End_RedKid();//ending가기 전 대화마지막끝날때호출

public:
    void        Set_Use_CameraEvent(bool b) { m_bUseCameraEvent=b; }

private:
    State       m_iState = State::WAIT;
    bool        m_bStartEvent = false;

    GameEvent       CameraEvent;
    bool        m_bUseCameraEvent = false;

private:
    _float4         m_vInitPosition = _float4(0.f, 0.f, 0.f, 1.f);

};

NS_END
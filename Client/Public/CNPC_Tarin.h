#pragma once
#include "CNPC.h"

NS_BEGIN(Client)
class CNPC_Tarin :
    public CNPC
{
public:
    enum class State
    {
       
        WAIT,WALK,TALK,TELEPORT,END
    };
protected:
    CNPC_Tarin(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CNPC_Tarin(const CNPC_Tarin& rhs);
    virtual ~CNPC_Tarin() = default;

public:
    HRESULT     Initialize_Prototype(void* pArg); /*NPC는 clone으로 생성하지않는다.*/

    virtual void        Update_Priority(_float fTimeDelta);
    virtual void        Update(_float fTimeDelta);
    virtual void        Update_Late(_float fTimeDelta);
    virtual void        Update_Render(_float fTimeDelta) override;


    virtual HRESULT Render();

public:
    ///범위감지용///
    virtual void        Enter_InteractRange();

    virtual void    Exit_Interaction();
public:
    void        Set_StartEvent(bool b, CNPC_Tarin::State iState) { CheckTrue(m_bStartEvent); m_bStartEvent = b; m_iState = iState; }
    void        Start_SecondChapter();
public:
    CNavigation* Get_Navigation() { return m_pNavigationCom; }
public:
    void        Update_State(_float fTimeDelta);

public:
    static CNPC_Tarin* Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContex, void* pArg);
    virtual void Free() override;

private:
    State       m_iState = State::WAIT;
    bool        m_bStartEvent = false;

    GameEvent       CameraEvent;


};
NS_END

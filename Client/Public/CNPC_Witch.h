#pragma once
#include "CNPC.h"

NS_BEGIN(Client)
class CSocket_Model;

class CNPC_Witch :
    public CNPC
{
protected:
    CNPC_Witch(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CNPC_Witch(const CNPC_Witch& rhs);
    virtual ~CNPC_Witch() = default;

public:
    HRESULT     Initialize_Prototype(void* pArg); /*NPC는 clone으로 생성하지않는다.*/

    virtual void        Update_Priority(_float fTimeDelta);
    virtual void        Update(_float fTimeDelta);
    virtual void        Update_Late(_float fTimeDelta);
    virtual void        Update_Render(_float fTimeDelta) override;
   

    virtual HRESULT Render();

private:
    void        Reigster_AnimNotify();
    void        Ready_Events();
    HRESULT     Create_SocketObject();
    HRESULT        Register_Listners();

public:
    /// ///실제 인터렉션 할때 호출되는함수

    virtual void        Enter_Interaction();
    virtual void        Stay_InteractRange(_float fTimeDelta);
    virtual void        Exit_Interaction();
public:
    static CNPC_Witch* Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContex, void* pArg);
    virtual void Free() override;


private:
    GameEvent Rotate_Camera_Event;
    GameEvent Rotate_Camera_End_Event;

    _float3 Witch_Camera_Rotation = _float3(50.f, 0.f, 0.f);
    bool        m_bEnd = false;

    CSocket_Model*        m_pSocketMushroom = nullptr;
    CSocket_Model*        m_pMagicPowder = nullptr;


};

NS_END
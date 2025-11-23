#pragma once
#include "CNPC.h"

NS_BEGIN(Client)
class CNPC_Richard :
    public CNPC
{
protected:
    CNPC_Richard(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CNPC_Richard(const CNPC_Richard& rhs);
    virtual ~CNPC_Richard() = default;

public:
    HRESULT     Initialize_Prototype(void* pArg); /*NPC는 clone으로 생성하지않는다.*/

    virtual void        Update_Priority(_float fTimeDelta);
    virtual void        Update(_float fTimeDelta);
    virtual void        Update_Late(_float fTimeDelta);
    virtual void        Update_Render(_float fTimeDelta) override;


    virtual HRESULT Render();

    virtual void    EnterInteractRange();
    virtual void    OnInteractRange(_float fTimeDelta);       //상호작용가능한 범위에잇을떄 계속호출
    virtual void    ExitInteractRange();


    virtual void    Start_Interaction();
    virtual void    On_Interaction(_float fTimeDelta);
    virtual void    Exit_Interaction();



public:
    static CNPC_Richard* Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContex, void* pArg);
    virtual void Free() override;


private:

    float       m_fTime = 0.f;//임시로 Exit조건 시간으로두기
};
NS_END

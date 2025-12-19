#pragma once
#include "CTrigger_Box.h"

NS_BEGIN(Client)
class CEventTrigger :
    public CTrigger_Box
{
public:
    struct EventTriggerDesc :public CTrigger_Box::TriggerBoxDesc
    {
        function<void()>    Func = nullptr;
    };

protected:
    CEventTrigger(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CEventTrigger(const CEventTrigger& rhs);
    virtual ~CEventTrigger() = default;

public:
    virtual HRESULT     Initialize_Prototype(); /*원형 객체가 생성될때 부르는 Initialize*/
    virtual HRESULT     Initialize_Copytype(void* pArg); /*사본 객체가 생성될때 부르는 Initialize*/

public:
    static CEventTrigger* Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContex);
    virtual CGameObject* Clone(void* pArg);
    virtual void    Free() override;

public:
    virtual     void    OnCollisionEnter(_uint iGroup, CCollider_Base * pOther);
    virtual     void    OnCollisionStay(_uint iGroup, CCollider_Base * pOther);
    virtual     void    OnCollisionExit(_uint iGroup, CCollider_Base * pOther);

private:
    function<void()>        m_Function = nullptr;



};
NS_END


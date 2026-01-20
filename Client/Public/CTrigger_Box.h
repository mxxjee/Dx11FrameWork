#pragma once
#include "CGameObject.h"

namespace Engine
{
    class CBoxColliderComponent;
}

NS_BEGIN(Client)
class CTrigger_Box :
    public CGameObject
{
public:
    struct TriggerBoxDesc :public CGameObject::GAMEOBJECT_DESC
    {
        _float3 vCenter;
        _float3 vExtents;
    };

protected:
    CTrigger_Box(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CTrigger_Box(const CTrigger_Box& rhs);
    virtual ~CTrigger_Box() = default;

public:
    virtual HRESULT     Initialize_Prototype(); /*원형 객체가 생성될때 부르는 Initialize*/
    virtual HRESULT     Initialize_Copytype(void* pArg); /*사본 객체가 생성될때 부르는 Initialize*/

    virtual void        Update_Priority(_float fTimeDelta);
    virtual void        Update(_float fTimeDelta);
    virtual void        Update_Late(_float fTimeDelta);
    virtual void        Update_Render(_float fTimeDelta);


    virtual HRESULT Render();

public:
    HRESULT     Ready_Components(void* pArg);
    CBoxColliderComponent* Get_Collider() { return pBoxCollider; }
    virtual     void    PushOut(_float3 vOutPush);

public:
    static CTrigger_Box* Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContex);
    virtual CGameObject* Clone(void* pArg);
    virtual void    Free() override;

protected:
    CBoxColliderComponent* pBoxCollider = nullptr;
};

NS_END


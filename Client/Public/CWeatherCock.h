#pragma once

#include "CAnimModelObject.h"
namespace  Engine
{
    class CBoxColliderComponent;

}
NS_BEGIN(Client)
class CWeatherCock :
    public CAnimModelObject
{

protected:
    CWeatherCock(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CWeatherCock(const CWeatherCock& rhs);
    virtual ~CWeatherCock() = default;


public:
    virtual HRESULT     Initialize_Prototype(void* pArg); /*원형 객체가 생성될때 부르는 Initialize*/

    virtual void        Update_Priority(_float fTimeDelta);
    virtual void        Update(_float fTimeDelta);
    virtual void        Update_Late(_float fTimeDelta);
    virtual void        Update_Render(_float fTimeDelta) override;


    virtual HRESULT Render();
    
public:
    virtual HRESULT     Ready_Components(void* pArg);
    virtual HRESULT     Ready_PartObjects(void* pArg);
    
public:
    static CWeatherCock* Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContex, void* pArg);
    virtual void Free() override;

private:
    CBoxColliderComponent* m_pCollider = nullptr;


};
NS_END

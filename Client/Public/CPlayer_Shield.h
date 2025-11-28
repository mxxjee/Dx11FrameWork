#pragma once
#include "CWeapon.h"

NS_BEGIN(Client)
class CPlayer_Shield :
    public CWeapon
{
protected:
    CPlayer_Shield(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CPlayer_Shield(const CPlayer_Shield& Prototype);
    virtual ~CPlayer_Shield() = default;

public:
    virtual HRESULT     Initialize_Prototype(); /*원형 객체가 생성될때 부르는 Initialize*/
    virtual HRESULT     Initialize_Copytype(void* pArg); /*사본 객체가 생성될때 부르는 Initialize*/

    virtual void        Update_Priority(_float fTimeDelta);
    virtual void        Update(_float fTimeDelta);
    virtual void        Update_Late(_float fTimeDelta);
    virtual void        Update_Render(_float fTimeDelta);

    virtual HRESULT Render();
private:
    HRESULT Ready_Components(void* pArg);

public:
    static CPlayer_Shield* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;


private:
    _float3 vTmp = _float3(0.f, 0.f, 0.f);

};

NS_END
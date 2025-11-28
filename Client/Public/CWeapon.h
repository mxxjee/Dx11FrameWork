#pragma once
#include "CPartObject.h"
namespace Engine
{
    class CCollider_Base;
}

NS_BEGIN(Client)
class CWeapon :
    public CPartObject
{
public:
    typedef struct tagWeaponDesc : public CPartObject::PARTOBJECT_DESC
    {
        //const _uint* pParentState = { nullptr };
        const _float4x4* pSocketMatrix = { nullptr };
    }WEAPON_DESC;


protected:
    CWeapon(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CWeapon(const CWeapon& Prototype);
    virtual ~CWeapon() = default;

public:
    virtual HRESULT     Initialize_Prototype(); /*원형 객체가 생성될때 부르는 Initialize*/
    virtual HRESULT     Initialize_Copytype(void* pArg); /*사본 객체가 생성될때 부르는 Initialize*/

    virtual void        Update_Priority(_float fTimeDelta);
    virtual void        Update(_float fTimeDelta);
    virtual void        Update_Late(_float fTimeDelta);
    virtual void        Update_Render(_float fTimeDelta);

    virtual HRESULT Render();

protected:
    vector<CCollider_Base*> m_pCollider = { nullptr };

protected:
    const _float4x4* m_pSocketMatrix = { nullptr };

    
    

public:
    static CWeapon* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;


};
NS_END


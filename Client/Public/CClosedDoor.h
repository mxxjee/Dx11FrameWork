#pragma once
#include "CStaticModelObject.h"
namespace Engine
{
    class CBoxColliderComponent;

}
NS_BEGIN(Client)
class CClosedDoor :
    public CStaticModelObject
{
protected:
    CClosedDoor(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CClosedDoor(const CClosedDoor& rhs);
    virtual ~CClosedDoor() = default;

public:
    virtual HRESULT     Initialize_Prototype(); /*원형 객체가 생성될때 부르는 Initialize*/
    virtual HRESULT     Initialize_Copytype(void* pArg); /*원형 객체가 생성될때 부르는 Initialize*/

    virtual void        Update_Priority(_float fTimeDelta);
    virtual void        Update(_float fTimeDelta);
    virtual void        Update_Late(_float fTimeDelta);
    virtual void        Update_Render(_float fTimeDelta) override;


    virtual HRESULT Render();
public:
    virtual HRESULT     Ready_Components(void* pArg);
    virtual HRESULT     Ready_PartObjects(void* pArg);

public:
    virtual     void    OnCollisionEnter(_uint iGroup, CCollider_Base* pOther);
    void            Close();
    void            Open();

public:
    static CClosedDoor* Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContex);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
private:
    CBoxColliderComponent* m_pCollider = nullptr;

    bool            m_bClosedUpdate = false;
    bool            m_bOpenUpdate = false;

    _vector         m_vClosePos;
    _vector         m_vInitPos;

};
NS_END

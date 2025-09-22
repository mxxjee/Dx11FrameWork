#pragma once
#include "CBase.h"

NS_BEGIN(Engine)
class CGameObject :
    public CBase
{
protected:
    CGameObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CGameObject(const CGameObject& rhs);
    ~CGameObject() = default;

public:
    virtual HRESULT     Initialize_Prototype();/*원형 객체 만들때의 Initialize*/
    virtual HRESULT     Initialize_Copytype(void* pArg);  /*사본객체 만들때의 Initialize*/

    virtual void        Update_Priority(_float fTimeDelta);
    virtual void        Update(_float fTimeDelta);
    virtual void       Update_Late(_float fTimeDelta);


    void        Render();


protected:
    ComPtr<ID3D11Device>            m_pDevice = { nullptr };
    ComPtr<ID3D11DeviceContext>     m_pDeviceContext = { nullptr };

public:
    virtual     CGameObject* Clone(void* pArg)=0;         //pArg : 추가적인 데이터
    virtual     void        Free() override;



};
NS_END
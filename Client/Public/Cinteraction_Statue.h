#pragma once
#include "CInteractionObject.h"

NS_BEGIN(Client)
class Cinteraction_Statue :
    public CInteractionObject
{
protected:
    Cinteraction_Statue(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    Cinteraction_Statue(const Cinteraction_Statue& rhs);
    virtual ~Cinteraction_Statue() = default;


public:
    virtual HRESULT     Initialize_Copytype(void* pArg); /*사본 객체가 생성될때 부르는 Initialize*/
    virtual HRESULT     Ready_PartObjects(void* pArg);
    virtual void        Update(_float fTimeDelta);


public:
    virtual bool IsInteratable() override;
    virtual void Enter_InteractRange() override;
    virtual void Exit_InteractRange() override;

    virtual void Enter_Interaction() override;
    virtual void Stay_Interaction(_float fTimeDelta) override;
    virtual void Exit_Interaction() override;

public:
    static Cinteraction_Statue* Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext);
    // CGameObject을(를) 통해 상속됨
    virtual CGameObject* Clone(void* pArg) override;
    virtual     void    Free() override;

private:
    class     CInteraction_TriggerBox* m_pTriggerBox = nullptr;


};
NS_END


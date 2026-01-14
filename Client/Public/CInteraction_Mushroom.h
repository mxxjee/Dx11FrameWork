#pragma once
#include "CInteractionObject.h"
NS_BEGIN(Client)
class CEffect;

class CInteraction_Mushroom :
    public CInteractionObject
{
    CInteraction_Mushroom(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CInteraction_Mushroom(const CInteraction_Mushroom& rhs);
    virtual ~CInteraction_Mushroom() = default;

public:
    virtual HRESULT     Initialize_Copytype(void* pArg); /*사본 객체가 생성될때 부르는 Initialize*/
    virtual HRESULT     Ready_PartObjects(void* pArg);

    virtual void        Update(_float fTimeDelta);
    virtual void        Update_Render(_float fTimeDelta);
    virtual HRESULT     Render();

public:
    virtual bool IsInteratable() override;
    virtual void Enter_InteractRange() override;
    virtual void Exit_InteractRange() override;

    virtual void Enter_Interaction() override;
    virtual void Stay_Interaction(_float fTimeDelta) override;
    virtual void Exit_Interaction() override;



public:
public:
    static CInteraction_Mushroom* Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext);
    // CGameObject을(를) 통해 상속됨
    virtual CGameObject* Clone(void* pArg) override;
    virtual     void    Free() override;


private:
    class     CInteraction_TriggerBox* m_pTriggerBox = nullptr;
    CEffect* m_pEffect = nullptr;


};
NS_END

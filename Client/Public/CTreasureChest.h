#pragma once
#include "CInteractionObject.h"
#include "Client_Defines.h"
#include "CIInteractable.h"

namespace Engine
{
    class CCollider_Base;
    class CNavigation;
    class CBody;
}

NS_BEGIN(Client)
class     CInteraction_TriggerBox;

class CTreasureChest :
    public CInteractionObject
{
public:
    typedef struct CTreasureChest_Desc:CInteractionObject::Interaction_DESC
    {
        ITMINFO* m_pInnerItem = nullptr;
    }TREASURECHEST_DESC;


protected:
    CTreasureChest(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CTreasureChest(const CTreasureChest& rhs);
    virtual ~CTreasureChest() = default;

public:
    HRESULT             Initialize_Prototype();
    virtual HRESULT     Initialize_Copytype(void* pArg); /*사본 객체가 생성될때 부르는 Initialize*/
    virtual HRESULT     Ready_PartObjects(void* pArg);



    virtual void        Update_Priority(_float fTimeDelta);
    virtual void        Update(_float fTimeDelta);
    virtual void        Update_Late(_float fTimeDelta);
    virtual void        Update_Render(_float fTimeDelta) override;


    virtual HRESULT Render();

public:
    static CTreasureChest* Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContex);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;

public:
    // CIInteractable을(를) 통해 상속됨
    virtual bool IsInteratable() override;

    virtual void Enter_InteractRange() override;

    virtual void Stay_InteractRange(_float fTimeDelta) override;

    virtual void Exit_InteractRange() override;

    virtual void Enter_Interaction() override;

    virtual void Stay_Interaction(_float fTimeDelta) override;

    virtual void Exit_Interaction() override;

    virtual _int Get_Interaction_Priority() override;

private:
    class       CInteraction_TriggerBox* m_pTriggerBox = nullptr;
    ITMINFO*    m_pInnerItem = nullptr;

    bool        m_bOpen = false;
    CBody*      m_pBodyBottom = nullptr;
};



NS_END
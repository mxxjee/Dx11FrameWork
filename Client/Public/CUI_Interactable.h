#pragma once
#include "CUI.h"

NS_BEGIN(Client)
class CUI_Interactable :
    public CUI
{
public:
    enum UIState {NONE,HOVER,SELECT,END};

protected:
    CUI_Interactable(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext);
    CUI_Interactable(const CUI_Interactable& rhs);
    virtual ~CUI_Interactable() = default;


public:
    virtual HRESULT Initialize_Prototype() override; /* 원형객체가 만들어질 때 호출되는 함수. 무거운 초기화작업.  */
    virtual HRESULT Initialize_Copytype(void* pArg) override; /* 원형을 복제하여 만든 사본객체가 생성시에 호출되는 함수. 추가적인 초기화를 위해 */

    virtual void Update_Priority(_float fTimeDelta) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Update_Late(_float fTimeDelta) override;
    virtual void Update_Render(_float fTimeDelta) override;

    virtual HRESULT Render() override;

public:
    static CUI_Interactable* Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;

public:
    /*셰이더에게 값을 어떻게보낼건지를 정의한다.*/
    virtual void OnHoverEnter();
    virtual void OnHoverExit();
    virtual void OnPress();
    virtual void OnRelease();
    virtual void OnDisabled();

private:
    UIState     m_eState = UIState::END;

};

NS_END


#pragma once
#include "CPanel.h"

namespace Engine
{
    class CInput_Manager;

}
NS_BEGIN(Client)
class CButton :
    public CPanel
{
public:
    typedef struct tagButtonDesc : public CUI::tagUIDesc
    {
        KeyCode eKeyCode = KeyCode::None;
        std::function<void()>   SelectActionFunc = nullptr;

    }BUTTON_DESC;
protected:
    CButton(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext);
    CButton(const CButton& rhs);
    virtual ~CButton() = default;


public:
    virtual HRESULT Initialize_Prototype() override; /* 원형객체가 만들어질 때 호출되는 함수. 무거운 초기화작업.  */
    virtual HRESULT Initialize_Copytype(void* pArg) override; /* 원형을 복제하여 만든 사본객체가 생성시에 호출되는 함수. 추가적인 초기화를 위해 */

    virtual void Update_Priority(_float fTimeDelta) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Update_Late(_float fTimeDelta) override;
    virtual void Update_Render(_float fTimeDelta) override;

    virtual HRESULT Render() override;

public:
    static CButton* Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
    
public:
    void            Set_Hover(bool b) { m_bHover = b; }
private:
    std::function<void()>       m_OnSelectAction=nullptr;        //클릭 이벤트

    bool                        m_bHover = false;               //호버

    KeyCode                     m_eKeyCode=KeyCode::None;     //선택하기위한 키, 이거 누르면 선택이다.
    
    class CInput_Manager* m_pInput_Manager = nullptr;

};
NS_END


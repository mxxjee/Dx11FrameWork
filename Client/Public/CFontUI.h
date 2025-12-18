#pragma once
#include "CUI.h"

namespace Engine
{
    class CFontComponent;
}
NS_BEGIN(Client)

class CFontUI :
    public CUI
{
public:
    typedef struct tagFontUIDesc : public CUI::tagUIDesc
    {
        wstring FontName;
        _float4 vDefaultFontColor = _float4(0.f, 0.f, 0.f, 1.f);//기본색 검은색
        _float  fRotation = 0.f;
        bool    m_bUseTypingEffect = false;
        _float  m_fTypingTime = 0.05f;

    }FONTUI_DESC;
protected:
    CFontUI(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext);
    CFontUI(const CFontUI& rhs);
    virtual ~CFontUI() = default;

public:
    virtual HRESULT Initialize_Prototype() override; /* 원형객체가 만들어질 때 호출되는 함수. 무거운 초기화작업.  */
    virtual HRESULT Initialize_Copytype(void* pArg) override; /* 원형을 복제하여 만든 사본객체가 생성시에 호출되는 함수. 추가적인 초기화를 위해 */


    virtual void Update_Priority(_float fTimeDelta) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Update_Late(_float fTimeDelta) override;
    virtual void Update_Render(_float fTimeDelta) override;

    virtual HRESULT Render() override;

public:
    void        Set_FontStartFunction(function<void()> func);
    void        Set_FontEndFunction(function<void()> func);
public:
    void            Set_Text(const wstring& Text);
    HRESULT         Ready_Component(void* pArg);
public:
    static CFontUI* Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;

private:
    CFontComponent* m_pFontComp = nullptr;
};

NS_END


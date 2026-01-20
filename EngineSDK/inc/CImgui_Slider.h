#pragma once
#include "CImgui_Widget.h"

NS_BEGIN(Engine)
class ENGINE_DLL CImgui_Slider :
    public CImgui_Widget
{
public:
    typedef struct ImguiSlider_Desc : public CImgui_Widget::IMGUIWIDGET_DESC
    {
        string      m_LabelName;    //슬라이더 이름
        float*      fValue = nullptr;       //바인딩할 값
        float       vMin;       //슬라이더 최소값
        float       vMax;       //슬라이더 최대값

        function<void()> callback = nullptr;

    }IMGUISLIDER_DESC;

protected:
    CImgui_Slider(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    virtual ~CImgui_Slider() = default;

public:
    // CImgui_Base을(를) 통해 상속됨
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Update() override;
    virtual void Render() override;

public:
    void        Set_BindValue(float* fValue) { m_fBindValue = fValue; }
    void        Set_Min(float _Min) { m_vMin = _Min; }
    void        Set_Max(float _Max) { m_vMax = _Max; }
    void        Set_Callback(function<void()> callback) { m_Callback = callback;   }

public:
    _float      Get_BindValue() { return *m_fBindValue; }
public:
    static CImgui_Slider* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg);

protected:
    string  m_Label;
    float*  m_fBindValue = { nullptr };
    float   m_vMin = 0;
    float   m_vMax = 1;

    function<void()> m_Callback = nullptr;


};
NS_END


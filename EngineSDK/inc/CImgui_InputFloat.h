#pragma once
#include "CImgui_Widget.h"

NS_BEGIN(Engine)
class  ENGINE_DLL CImgui_InputFloat :
    public CImgui_Widget
{
public:
    typedef struct ImguiTextFloatDesc : public CImgui_Widget::IMGUIWIDGET_DESC
    {
        string Label;
        function<void()> callback;
        float* pData;
        float             Step = 1;

    }IMGUITEXTFLOAT_DESC;

protected:
    CImgui_InputFloat(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    virtual ~CImgui_InputFloat() = default;

public:
    // CImgui_Base을(를) 통해 상속됨
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Update() override;
    virtual void Render() override;

public:
    void            Set_Callback(function<void()> callBack) { m_Callback = callBack; }

public:
  
    static CImgui_InputFloat* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg);

private:
    string m_Label = "";
    function<void()> m_Callback;
    float* m_pData;
    float m_Step = 0;
};

NS_END


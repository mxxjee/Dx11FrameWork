#pragma once
#include "CImgui_Widget.h"

NS_BEGIN(Engine)
class ENGINE_DLL CImgui_InputText :
    public CImgui_Widget
{
public:
    typedef struct ImguiTextInputDesc : public CImgui_Widget::IMGUIWIDGET_DESC
    {

        string Label;
        function<void()> callback;

       
    }IMGUITEXTINPUT_DESC;

protected:
    CImgui_InputText(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    virtual ~CImgui_InputText() = default;

public:
    // CImgui_Base을(를) 통해 상속됨
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Update() override;
    virtual void Render() override;

public:
    void            Set_Callback(function<void()> callBack) { m_Callback = callBack; }

public:
    static CImgui_InputText* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg);
private:
    char buf[MAX_PATH] = "";
    string m_Label = "";
    function<void()> m_Callback;
};
NS_END


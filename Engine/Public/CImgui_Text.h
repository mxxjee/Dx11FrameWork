#pragma once
#include "CImgui_Widget.h"
class CImgui_Text :
    public CImgui_Widget
{
public:
    typedef struct ImguiText_Desc : public  CImgui_Widget::IMGUIWIDGET_DESC
    {
        function<string()> TextCallback;

    }IMGUITEXT_DESC;

protected:
    CImgui_Text(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    virtual ~CImgui_Text() = default;

public:
    // CImgui_Base을(를) 통해 상속됨
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Update() override;
    virtual void Render() override;


public:
    static CImgui_Text* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg);

protected:
    function<string()> m_Callback = nullptr;
};


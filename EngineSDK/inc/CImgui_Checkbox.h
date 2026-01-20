#pragma once
#include "CImgui_Widget.h"

NS_BEGIN(Engine)
class ENGINE_DLL CImgui_Checkbox :
    public CImgui_Widget
{

public:
    typedef struct ImguiCheckbox_Desc : public CImgui_Widget::IMGUIWIDGET_DESC
    {

        string Label;
        function<void()> callback;
        bool* bValue = nullptr;
        function<bool*()> bValueGetter = nullptr;

        ImguiCheckbox_Desc()
            :Label(""), callback(nullptr), bValue(nullptr)
        {

        }
    }IMGUICHECKBOX_DESC;

protected:
    CImgui_Checkbox(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    virtual ~CImgui_Checkbox() = default;

public:
    // CImgui_Base을(를) 통해 상속됨
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Update() override;
    virtual void Render() override;


public:
    void            Set_Callback(function<void()> callBack) { m_Callback = callBack; }

public:
    static CImgui_Checkbox* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg);


protected:
    string          m_Label;
    bool            dummy = false;

    bool*           m_bValue = nullptr;
    function<bool* ()> bValueGetter = nullptr;

    function<void()> m_Callback = nullptr;


};
NS_END


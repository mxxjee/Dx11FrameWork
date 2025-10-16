#pragma once
#include "CImgui_Base.h"



NS_BEGIN(Engine)

class CImgui_Widget;

class ENGINE_DLL CImgui_Window :
    public CImgui_Base
{
public:
    typedef struct ImguiWindow_Desc : public CImgui_Base::IMGUI_DESC
    {
        ImVec2							m_WindowPos;
        ImVec2							m_WindowSize;
        vector<CImgui_Widget*>          m_Widgets;
        string	m_WindowTitle = "";

    }IMGUIWINDOW_DESC;
protected:
    CImgui_Window(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    virtual ~CImgui_Window() = default;

public:
    // CImgui_Base을(를) 통해 상속됨
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Update() override;
    virtual void Render() override;

public:
    template <typename T>
    HRESULT         Add_Widgets(void* pArg, CImgui_Widget** ppOut)
    {
        *ppOut = T::Create(m_pDevice, m_pContext, pArg);
        m_vWidgets.push_back(*ppOut);
        Safe_AddRef(*ppOut);
        return S_OK;
    }
public:
    static CImgui_Window* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg);
    virtual void Free();

protected:
    vector<CImgui_Widget*>          m_vWidgets;
    string  m_WindowTitle = "";     //창 타이틀
    bool    m_bOpen = true;        //창 열림,닫힘 여부
};

NS_END


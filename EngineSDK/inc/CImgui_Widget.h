#pragma once
#include "CImgui_Base.h"

NS_BEGIN(Engine)
class ENGINE_DLL CImgui_Widget abstract:
    public CImgui_Base
{
public:
    typedef struct ImguiWidget_Desc :public CImgui_Base::IMGUI_DESC
    {
        ImVec2      m_RelativePos;
        ImVec2      m_Size;
    }IMGUIWIDGET_DESC;
protected:
    CImgui_Widget(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    virtual ~CImgui_Widget() = default;


public:
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Update() override ;
    virtual void Render() override ;
public:
    virtual void Free();

private:
    ImVec2      RelativePos;
 
};

NS_END


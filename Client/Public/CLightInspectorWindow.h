#pragma once
#include "CImgui_Window.h"

namespace Engine
{
    class CLight;

}

NS_BEGIN(Client)
class CLightInspectorWindow :
    public CImgui_Window
{
protected:
    CLightInspectorWindow(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    virtual ~CLightInspectorWindow() = default;


public:
    // CImgui_Base을(를) 통해 상속됨
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Update() override;
    virtual void Render() override;

public:
    void            Set_SelectObject(class CLight* pTarget);
    virtual         void    Reset() { pSelectObject = nullptr; }        //리셋하고싶은 값 리셋하기

public:
    static CLightInspectorWindow* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg);
    virtual void Free();

private:
    CLight* pSelectObject = { nullptr };
   
};

NS_END
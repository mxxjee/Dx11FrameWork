#pragma once
#include "CImgui_Window.h"

namespace Engine
{
    class CGameObject;
}

NS_BEGIN(Client)

class CObjectDebugWindow :
    public CImgui_Window
{
protected:
    CObjectDebugWindow(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    virtual ~CObjectDebugWindow() = default;


public:
    // CImgui_Base을(를) 통해 상속됨
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Update() override;
    virtual void Render() override;

public:
    void            Set_SelectObject(class CGameObject* pTarget) { pSelectObject = pTarget; }
public:
    static CObjectDebugWindow* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg);
    virtual void Free();

private:
    CGameObject* pSelectObject = { nullptr };

};

NS_END
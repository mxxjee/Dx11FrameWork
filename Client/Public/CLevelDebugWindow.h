#pragma once
#include "CImgui_Window.h"

namespace Engine
{
    class CGameInstance;
}
NS_BEGIN(Client)
class CLevelDebugWindow :
    public CImgui_Window
{
protected:
    CLevelDebugWindow(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    virtual ~CLevelDebugWindow() = default;
    
public:
    // CImgui_Base을(를) 통해 상속됨
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Update() override;
    virtual void Render() override;

public:
    static CLevelDebugWindow* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg);
    virtual void Free();


private:
    CGameInstance* pGameInstance = { nullptr };

};
NS_END


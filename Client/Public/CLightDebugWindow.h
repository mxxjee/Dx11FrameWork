#pragma once
#include "CImgui_Window.h"

namespace Engine
{
    class CLight;
    class CGameInstance;

}

NS_BEGIN(Client)
class CLightDebugWindow :
    public CImgui_Window
{
protected:
    CLightDebugWindow(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    virtual ~CLightDebugWindow() = default;


public:
    // CImgui_Base을(를) 통해 상속됨
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Update() override;
    virtual void Render() override;

public:
    void        Save();
    void        Load();
    void        Add_Lights();

public:
    static CLightDebugWindow* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg);
    virtual void Free();

private:
    CGameInstance* m_pGameInstance = nullptr;

};
NS_END

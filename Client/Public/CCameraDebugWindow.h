#pragma once
#include "CImgui_Window.h"

namespace Engine
{
    class CImgui_Button;
    class CImgui_Slider;
    class CGameInstance;
}

NS_BEGIN(Client)

class CCameraDebugWindow :
    public CImgui_Window
{
private:
    enum SliderType
    {
        NearClip,
        FarClip,
        FOV,
        END
    };
protected:
    CCameraDebugWindow(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    virtual ~CCameraDebugWindow() = default;


public:
    // CImgui_Base을(를) 통해 상속됨
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Update() override;
    virtual void Render() override;

private:
    HRESULT     Create_Widgets();
    HRESULT     Create_Button(void* pArg, CImgui_Button** ppOut);
    HRESULT     Create_Slider(void* pArg, CImgui_Slider** ppOut);


private:
    void            ShowMainCameraDebug(bool isOrtho);
    void            ToggleClickOrtho(bool _b);

public:
    static CCameraDebugWindow* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg);
    virtual void Free();


private:
    CImgui_Button* m_pPerspectiveCamButton = { nullptr };
    CImgui_Button* m_pOrthoGraphicCamButton = { nullptr };
    
    vector<CImgui_Slider*> m_Sliders;

private:
    CGameInstance* m_pGameInstance = { nullptr };
    int            m_bClickOrtho = -1;

    float fDebugNear = 0.f;
    float fDebugFar = 0.f;
    float fDebugFov = 0.f;
};

NS_END
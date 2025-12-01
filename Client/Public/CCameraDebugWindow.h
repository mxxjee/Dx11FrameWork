#pragma once
#include "CImgui_Window.h"

namespace Engine
{
    class CImgui_Button;
    class CImgui_Slider;
    class CImgui_Checkbox;
    class CGameInstance;
    class CCamera_Base;
    class CImgui_InputFloat;

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
        OFFSET_X,OFFSET_Y,OFFSET_Z,
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


private:
    void            ShowMainCameraDebug(bool isOrtho);
    void            ToggleOffSetDebug();
    void            ToggleClickOrtho(bool _b);

public:
    static CCameraDebugWindow* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg);
    virtual void Free();


private:
    CImgui_Button* m_pPerspectiveCamButton = { nullptr };
    CImgui_Button* m_pOrthoGraphicCamButton = { nullptr };
    CImgui_Button* m_pDefaultOffSetButton = { nullptr };


    vector<CImgui_Slider*> m_Sliders;
    vector<CImgui_Button*> m_CamButtons;
    vector<CImgui_InputFloat*> m_InuptFloats;

    vector<CImgui_Checkbox*> m_CheckBoxs;

    CImgui_Slider*  m_CulDistSlider = nullptr;

private:
    CGameInstance* m_pGameInstance = { nullptr };
    int            m_bClickOrtho = -1;

    float fDebugNear = 0.f;
    float fDebugFar = 0.f;
    float fDebugFov = 0.f;
    float fCulDist = 50.f;


    _float3 fDebugOffSet;
    _float3 fDefaultOffSet;

    CCamera_Base*           pTargetCamera = nullptr;
    CCamera_Base*           m_pSelectCamera = nullptr;


};

NS_END
#pragma once
#include "CImgui_Window.h"

namespace Engine
{
    class CNavMeshEdit_Manager;
    class IMapEditable;
    class CImgui_Button;
}


NS_BEGIN(MapTool)
class CNavMeshDebugWindow :
    public CImgui_Window
{
protected:
    CNavMeshDebugWindow(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    virtual ~CNavMeshDebugWindow() = default;

private:
    HRESULT     Create_Widgets();

public:
    // CImgui_Base을(를) 통해 상속됨
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Update() override;
    virtual void Render() override;

    void    UpdatePoints(deque<PreviewPoint> Points);

private:
    class CImgui_Button* m_pButton;
    class CImgui_Button* m_pShowSoloCell_Button;
    class CImgui_Button* m_pSetPlane_Button;
public:
    static CNavMeshDebugWindow* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg);
    virtual void Free();

private:
    CNavMeshEdit_Manager* m_pNavMeshEdit_Manager = nullptr;
    bool        m_bShowCellToggle = false;
};

NS_END


#pragma once
#include "CImgui_Window.h"


namespace Engine
{
    class CGameInstance;
}

NS_BEGIN(MapTool)
class CImgui_MenuBar :
    public CImgui_Window
{
protected:
    CImgui_MenuBar(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    virtual ~CImgui_MenuBar() = default;

public:
    // CImgui_Base을(를) 통해 상속됨
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Update() override;
    virtual void Render() override;


public:
    static CImgui_MenuBar* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg);
    virtual void Free();


private:
    CGameInstance* pGameInstance = { nullptr };
    CMapObject_Manager* m_pMapObject_Manager = { nullptr };

};
NS_END


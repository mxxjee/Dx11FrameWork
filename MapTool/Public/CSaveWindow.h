#pragma once
#include "CImgui_Window.h"

namespace Engine
{
    class CGameInstance;
    class CImgui_Button;
    class CMapObject_Manager;

}
NS_BEGIN(MapTool)

class CSaveWindow :
    public CImgui_Window
{
protected:
    CSaveWindow(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    virtual ~CSaveWindow() = default;
public:
    // CImgui_Base을(를) 통해 상속됨
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Update() override;
    virtual void Render() override;

private:
    HRESULT     Create_Widgets();

    HRESULT     Load_SaveFiles();       //세이브파일을가져와서 리스트에 보여줄 아이템을 수집


    void        Show_ListBox();

public:
    static CSaveWindow* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg);
    virtual void Free();

private:
    CGameInstance* pGameInstance = { nullptr };
    CMapObject_Manager* pMapObject_Manager = nullptr;
private:
    CImgui_Button*      m_pSaveButton = nullptr;
    
    vector<string>              m_SaveFiles;
    vector<string>              m_SaveFileNames;
    vector<const char*>         m_SaveFileNamesStr;
    
    string           SavePath = "";
    int              LoadFileIdx = 0;
};
NS_END


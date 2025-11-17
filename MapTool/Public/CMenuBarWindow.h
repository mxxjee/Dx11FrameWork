#pragma once
#include "CImgui_Window.h"
#include "MapTool_Struct.h"


namespace Engine
{
    class CGameInstance;
    class CMapObject_Manager;

}

NS_BEGIN(MapTool)
class CImgui_DataManager;

class CMenuBarWindow :
    public CImgui_Window
{
protected:
    CMenuBarWindow(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    virtual ~CMenuBarWindow() = default;

public:
    // CImgui_Base을(를) 통해 상속됨
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Update_Priority() override;

    virtual void Update() override;
    virtual void Render() override;


public:
    static CMenuBarWindow* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg);
    virtual void Free();


private:
    void        Update_Menu();


private:
    void        Show_SaveMenu();        //저장
    void        Show_SaveAsMenu();      //다른이름으로 저장


    void        Show_LoadMenu();
    void        Show_ModeMenu();

    void        Show_ListBox();     //불러올목록표시
    void        Show_TextBox();     //다른이름으로 저장할 이름 적는곳

private:
    CGameInstance* pGameInstance = { nullptr };
    CMapObject_Manager* m_pMapObject_Manager = { nullptr };
    CImgui_DataManager* m_pImgui_DataManager = { nullptr };


private:
    SaveFilePath            m_SaveFilePath;         //Datamanager로부터 참조
    LoadFilePath            m_LoadFilePath;         //Datamanager로부터 참조

private:
    bool            m_bListOpen = false;
    bool            m_bTextOpen = false;
    char            m_szSaveName[MAX_PATH]="";
    bool            m_bLoad = false;
};
NS_END


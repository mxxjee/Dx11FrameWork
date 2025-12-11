#pragma once
#include "CImgui_Window.h"
#include "CFolder.h"

namespace Engine
{
    class CGameInstance;
    class CMapObject;
    class CMapObject_Manager;
}


NS_BEGIN(MapTool)
class CImgui_DataManager;

class CAssetListWindow :
    public CImgui_Window
{


protected:
    CAssetListWindow(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    virtual ~CAssetListWindow() = default;

public:
    HRESULT      Set_AssetList();


private:
    HRESULT     Create_Widgets();

public:
    HRESULT     Create_Folders();       //진입 이후에 불러야함.

public:
    // CImgui_Base을(를) 통해 상속됨
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Update() override;
    virtual void Render() override;
public:
    static CAssetListWindow* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg);
    virtual void Free();

private:
    HRESULT       Create_TerrainFile();
    HRESULT       Create_TileFile();
    HRESULT       Create_ModelFile();
    HRESULT       Create_Triggers();


private:
    void        Show_Grid(const string& Category, int FieldNum);
    CFolder*    Get_Folder(const char* FileName);

private:
    CGameInstance* pGameInstance = { nullptr };
    CMapObject_Manager* m_pMapObject_Manager = { nullptr };
    CImgui_DataManager* m_pImgui_DataManager = { nullptr };


private:
    UMap<_wstring, vector<CFolder*>> m_FolderMap;
    vector<tagAssetInfo>         TileImages;


    string             SelectCategory = "";
    int                 SelectIdx = -1;

};


NS_END


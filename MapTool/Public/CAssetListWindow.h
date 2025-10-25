#pragma once
#include "CImgui_Window.h"

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
public:
    typedef struct tagAssetInfo
    {
        wstring TexKey = L"";   //Texmgr에 이미등록해논 키
        wstring FullPath = L""; //전체경로
    }AssetInfo;

protected:
    CAssetListWindow(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    virtual ~CAssetListWindow() = default;


private:
    HRESULT     Create_Widgets();


public:
    // CImgui_Base을(를) 통해 상속됨
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Update() override;
    virtual void Render() override;
public:
    static CAssetListWindow* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg);
    virtual void Free();

private:
    HRESULT       Create_ModelImages();
private:
    void        Show_ModelGrid();
    void        Show_TileGrid();

private:
    CGameInstance* pGameInstance = { nullptr };
    CMapObject_Manager* m_pMapObject_Manager = { nullptr };
    CImgui_DataManager* m_pImgui_DataManager = { nullptr };


private:
    vector<tagAssetInfo>        ModelImages;
    vector<wstring>         TileImages;

};


NS_END


#pragma once
#include "CBase.h"
#include "MapTool_Struct.h"
//MAPTOOL프로젝트에서만 사용하는 임구이 데이터전달용

namespace Engine
{
    class CInput_Manager;
    class CMapObject_Manager;
    class CGameInstance;
    class CTerrain_Manager;
}
NS_BEGIN(MapTool)

class CGrid_Manager;
 
class CImgui_DataManager :
    public CBase
{
    DECLARE_SINGLETON(CImgui_DataManager);

private:
    explicit CImgui_DataManager();
    virtual ~CImgui_DataManager();
public:

    struct PlaceObjectInfo
    {
        MapObjType              ObjType;    //설치하려는 오브젝트타입
        ResourceType            m_resourceType;


        wstring                 TexKey = L"";   //아이콘에 표시되는 텍스처이름,이걸 기반으로 아이디생성할듯
        _uint                   idx = 0;



    };

    typedef struct tagImguiShared_Data
    {
        string          m_SelectedCategory="Model";
        string          m_SelectedItemName;

        int             SelectIdx = -1;
        bool            m_bPlacementMode = false;
    

    }IMGUI_SHARED_DATA;


public:
    HRESULT     Initialize(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pContext);

#pragma region Placement관련
public:
                            //폴더를 지정하면 ,폴더안의 목록 중 png/jpg/dds인 파일들의 절대경로 목록을 vector<>에 담아 반환
    vector<wstring>      GetImageFiles(const wstring& folderPath);
    
                            //AssetCategory어떤거 선택했는지 전송하는 함수
    void            Send_SelectedCategory(const string& Str) { Data.m_SelectedCategory = Str; }
    void            Send_SelectedIdx(int i) { Data.SelectIdx = i; }

    IMGUI_SHARED_DATA* Get_Data() { return &Data; }

public:
    void        Active_PlacementMode(PlaceObjectInfo Info);
    void        Update_MouseInput();     //마우스클릭,release에 따른 생성명령
    MapObjType      Get_ObjType_From_Path(const wstring& path);     //경로를 통해 objtype을 반환



private:
    HRESULT             Create_MapObject();
    HRESULT             Create_MapTerrain();
    HRESULT             Create_MapInteraction();
    HRESULT             Create_Model();



   
public:
    IMGUI_SHARED_DATA           Data;
    PlaceObjectInfo             m_PlaceObjInfo;
   class CGameObject* m_pPlaceObject;
#pragma endregion


#pragma region 저장/로드 관련

public:
    bool            IsLoadedTerrain() { return m_LoadFilePath.m_CurrentLoadTerrainFilePath != ""; }//로드한적이있냐. 그러면 덮어쓰기가능!
    bool            IsLoadedNav() { return m_LoadFilePath.m_CurrentLoadNavFilePath != ""; }
    bool            IsLoadInteraction() { return m_LoadFilePath.m_CurrentLoadInteractionFilePath != ""; }


    const SaveFilePath& Get_SaveFilePath() { return m_SaveFilePath; }
    const LoadFilePath& Get_LoadFilePath() { return m_LoadFilePath; }

    void            Set_SaveFilePath(const SaveFilePath& pFile) { m_SaveFilePath = pFile; }
    void            Set_LoadFilePath(const LoadFilePath& pFile) { m_LoadFilePath = pFile; }


public:
    HRESULT       Update_SaveFiles();       //저장된 파일들을 모두 검사하여 경로를벡터에담는다.
private:
    SaveFilePath            m_SaveFilePath;
    LoadFilePath            m_LoadFilePath;

#pragma endregion



public:
    void            Free() override;

private:
    CInput_Manager* m_pInputManager = nullptr;
    CMapObject_Manager* m_pMapObject_Manager = nullptr;
    CGrid_Manager* m_pGrid_Manager = nullptr;
    CGameInstance* m_pGameInstance = nullptr;

    bool                m_bDrag = false;

   
private:
    ComPtr<ID3D11Device> m_pDevice;
    ComPtr<ID3D11DeviceContext> m_pContext;


};
NS_END

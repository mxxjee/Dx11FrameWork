#pragma once
#include "CBase.h"

//MAPTOOL프로젝트에서만 사용하는 임구이 데이터전달용

NS_BEGIN(MapTool)
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
        wstring                 TexKey = L"";   //아이콘에 표시되는 텍스처이름,이걸 기반으로 아이디생성할듯



    };

    typedef struct tagImguiShared_Data
    {
        string          m_SelectedCategory;
        string          m_SelectedItemName;

        bool            m_bPlacementMode = false;
    

    }IMGUI_SHARED_DATA;


public:
                            //폴더를 지정하면 ,폴더안의 목록 중 png/jpg/dds인 파일들의 절대경로 목록을 vector<>에 담아 반환
    vector<wstring>      GetImageFiles(const wstring& folderPath);
    void            Send_SelectedCategory(const string& Str) { Data.m_SelectedCategory = Str; }

    IMGUI_SHARED_DATA* Get_Data() { return &Data; }

public:
    void        Active_PlacementMode(PlaceObjectInfo Info);
    void        Update_MouseInput();     //마우스클릭,release에 따른 생성명령

public:
    void            Free() override;

public:
    IMGUI_SHARED_DATA           Data;
    PlaceObjectInfo             m_PlaceObjInfo;
};
NS_END

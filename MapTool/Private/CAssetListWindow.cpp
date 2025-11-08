#include "CAssetListWindow.h"
#include "CGameInstance.h"
#include "CMapObject_Manager.h"
#include "CImgui_DataManager.h"
#include "CTexture.h"
#include "MapTool_Defines.h"
#include "CModel.h"
#include "CFolder.h"


USING(MapTool)
CAssetListWindow::CAssetListWindow(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CImgui_Window(pDevice, pContext),
    pGameInstance{ CGameInstance::GetInstance() },
    m_pMapObject_Manager{ CMapObject_Manager::GetInstance() },
    m_pImgui_DataManager{ CImgui_DataManager::GetInstance() }
{
    Safe_AddRef(pGameInstance);
    Safe_AddRef(m_pImgui_DataManager);
    Safe_AddRef(m_pMapObject_Manager);
}

HRESULT CAssetListWindow::Create_Widgets()
{
    return S_OK;
}

HRESULT CAssetListWindow::Create_Folders()
{
    /*폴더생성*/
    for (int i = 5; i <= 16; ++i)
    {

        //Field01
        string Name = "Field_" + to_string(i);

        CFolder::tagFolderDesc Desc;
        strcpy_s(Desc.Name, MAX_PATH, Name.c_str());
        Desc.Size = ImVec2(60.f, 60.f);
        Desc.iIdx = i - 5;
        Desc.Category = "Model";


        CFolder* pInstance = CFolder::Create(m_pDevice, m_pContext, &Desc);
        if (pInstance)
        {
            if (FAILED(pInstance->Initialize(&Desc)))
                return E_FAIL;

            m_Folders.push_back(pInstance);

        }
    }


    return S_OK;

}

HRESULT CAssetListWindow::Initialize(void* pArg)
{
    if(FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Create_Folders()))
        return E_FAIL;

    if (FAILED(Set_AssetList()))
        return E_FAIL;

    ModelImages.reserve(50);



    return S_OK;
}

HRESULT CAssetListWindow::Set_AssetList()
{
    //이미지 생성을 위해 경로다읽어오기.
    if (FAILED(Create_ModelImages()))
        return E_FAIL;


    if (FAILED(Create_TileImages()))
        return E_FAIL;

    return S_OK;
}

void CAssetListWindow::Update()
{
    ImGui::Begin(m_WindowTitle.c_str(), &m_bOpen);

    SelectCategory = m_pImgui_DataManager->Get_Data()->m_SelectedCategory;
    SelectIdx = m_pImgui_DataManager->Get_Data()->SelectIdx;


    ImGui::Columns(10,0, false);


    //선택안함 = SelectIdx=-1;
    if (SelectIdx == -1)
    {
        for (auto& p : m_Folders)
        {
            if (p->Update())
            {
                m_pImgui_DataManager->Send_SelectedIdx(p->Get_Idx());
            }

            ImGui::TextWrapped(p->Get_Name());
            ImGui::NextColumn();


        }

    }
  
    
    //선택한 카테고리에 따라서 표시
    Show_Grid(SelectCategory, SelectIdx);

    ImGui::End();
}

void CAssetListWindow::Render()
{
}

CAssetListWindow* CAssetListWindow::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg)
{
    CAssetListWindow* pInstance = new CAssetListWindow(pDevice, pContext);


    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Create : CAssetListWindow");
        Safe_Release(pInstance);
    }


    return pInstance;
}



HRESULT CAssetListWindow::Create_ModelImages()
{
    const UMap<_wstring, CModel*>       ModelMap = pGameInstance->Get_MapModel();

    for (auto& pair : ModelMap)
    {
        if (pair.second)
        {
            const ModelData modelData = pair.second->Get_ModelData();

            AssetInfo Info;


            wstring Key = modelData.name.substr(0, modelData.name.size()-1);
            string Result = WStringToUTF8(Key);


            
            if (modelData.ResourcePath.find(L"Field") != wstring::npos)
                Info.ObjType = MapObjType::TERRAIN;

            else
                Info.ObjType = MapObjType::OBSTACLE;

            Info.TexKey = modelData.name;

            Info.FullPath = modelData.ResourcePath;
            ModelImages.push_back(Info);

            CFolder* pFolder = Get_Folder(Result.c_str());
            if (pFolder)
                pFolder->Add_Info(Info);
        }

     
    }

    return S_OK;

  
}

HRESULT CAssetListWindow::Create_TileImages()
{
    vector<wstring> Tmp = m_pImgui_DataManager->GetImageFiles(L"C:/Users/kmj69/Documents/GitHub/DX11Framework/Resource/Tile");
    for (auto& i : Tmp)
    {
        AssetInfo info;
        info.FullPath = i;

        TileImages.push_back(info);
    }

    for (auto& info : TileImages)
    {
        fs::path    Tmp = info.FullPath;
        info.TexKey = Tmp.stem().wstring();

        CTexture* pTexture = CTexture::Create(m_pDevice, m_pContext, info.FullPath.c_str(), 1);
        if (FAILED(pGameInstance->Register_Texture(info.TexKey, pTexture)))
            return E_FAIL;

    }
}

void CAssetListWindow::Show_Grid(const string& Category,int FieldNum)
{
    CheckTrue(Category == "");
    CheckTrue(FieldNum == -1);

    int  columnCount = 10;
    ImVec2 iconSize = ImVec2(64, 64); // 썸네일 크기
    ImGui::Columns(columnCount, 0, false);
    vector<tagAssetInfo>*        Target;
    if (Category == "Model")
    {
        Target = m_Folders[FieldNum]->get_vector();
    }
        

    else
        Target = &TileImages;


    for (int i = 0; i < Target->size(); ++i)
    {
        ImGui::BeginGroup();
        // 썸네일 이미지
        AssetInfo info = Target->at(i);
        CTexture* pTex = pGameInstance->Find_Texture(info.TexKey);

        ComPtr<ID3D11ShaderResourceView> SRV = nullptr;

        if (pTex)
            SRV = pTex->Get_SRV(0);

        else
            SRV = pGameInstance->Find_Texture(L"Default")->Get_SRV(0);

        ImTextureID tex = (ImTextureID)SRV.Get();


        if (ImGui::ImageButton(WStringToUTF8(info.TexKey).c_str(), tex, iconSize))
        {


        }

        if (ImGui::IsItemActivated())
        {
            // 클릭 시 이벤트 처리
           //just 클릭시 -> 0,0,0에 바로생성
           //click->drag 시 마우스따라가면서 터레인에 생성, release 시 위치고정

            ///지금해야하는것 : 클릭한애들의 ObjType을 어케지정할거냐..
            CImgui_DataManager::PlaceObjectInfo PlaceInfo;
            PlaceInfo.ObjType = m_pImgui_DataManager->Get_ObjType_From_Path(info.FullPath);

            if (PlaceInfo.ObjType == MapObjType::TILE)
                PlaceInfo.m_resourceType = ResourceType::TEXTURE;

            else
                PlaceInfo.m_resourceType = ResourceType::MODEL;


            PlaceInfo.TexKey = info.TexKey;
            PlaceInfo.ObjType = info.ObjType;

            m_pImgui_DataManager->Active_PlacementMode(PlaceInfo);
        }



        // 파일 이름 밑에 표시
        std::string name = WStringToUTF8(info.TexKey);
        ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + 80);
        ImGui::TextWrapped(name.c_str());
        ImGui::PopTextWrapPos();

        ImGui::EndGroup();
        ImGui::NextColumn();

    }
}

CFolder* CAssetListWindow::Get_Folder(const char* FileName)
{
    for (auto& pFolder : m_Folders)
    {
        if (!strcmp(pFolder->Get_Name(), FileName))
            return pFolder;

    }

    return nullptr;
}

void CAssetListWindow::Free()
{
    __super::Free();
    for (auto& i : m_Folders)
        Safe_Release(i);

    Safe_Release(m_pMapObject_Manager);
    Safe_Release(m_pImgui_DataManager);
    Safe_Release(pGameInstance);
    
}
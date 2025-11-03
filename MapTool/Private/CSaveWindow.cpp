#include "CSaveWindow.h"
#include "CImgui_Button.h"
#include "CGameInstance.h"
#include "CMapObject_Manager.h"

USING(MapTool)
CSaveWindow::CSaveWindow(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CImgui_Window(pDevice,pContext)
{
}

HRESULT CSaveWindow::Initialize(void* pArg)
{
    SavePath = "../../Resource/Data/Map/";
    pGameInstance = CGameInstance::GetInstance();
    pMapObject_Manager = CMapObject_Manager::GetInstance();


    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (FAILED(Create_Widgets()))
        return E_FAIL;

    if (FAILED(Load_SaveFiles()))
        return E_FAIL;
    return S_OK;
}

void CSaveWindow::Update()
{

    __super::Update();

    ImGui::Begin(m_WindowTitle.c_str(), &m_bOpen);

    Show_ListBox();

    ///////ListBox여는 동안ㄱ ㄱ세이브파일 매프레임수집..?
    ImGui::End();

}

void CSaveWindow::Render()
{
}

HRESULT CSaveWindow::Create_Widgets()
{
    CImgui_Button::IMGUIBUTTON_DESC ImguiButtonDesc;
    ImguiButtonDesc.Tag = "SaveButton";
    ImguiButtonDesc.Label = ImguiButtonDesc.Tag;

    ImguiButtonDesc.m_RelativePos = ImVec2(30.f, 30.f);
    ImguiButtonDesc.callback = [&]()
    {
        //저장함수호출
        //obj.저장
        //terrain저장
        if (FAILED(CGameInstance::GetInstance()->Save_All_Terrains(SavePath, m_SaveFiles.size())))
            return;

 
        //Refresh SaveFileList
        if (FAILED(Load_SaveFiles()))
            return;

    };
    

    if (FAILED(Add_Widgets<CImgui_Button>(&ImguiButtonDesc, reinterpret_cast<CImgui_Widget**>(&m_pSaveButton))))
        return E_FAIL;


    return S_OK;
}

HRESULT CSaveWindow::Load_SaveFiles()
{
    m_SaveFiles.clear();
    m_SaveFileNames.clear();
    m_SaveFileNamesStr.clear();

    for (const auto& entry : fs::recursive_directory_iterator(SavePath))
    {
        if (entry.path().extension() == ".json")
        {
            std::string fullPath = entry.path().string();
            std::string fileName = entry.path().stem().string()+".json"; // 이름만 추출

            m_SaveFiles.push_back(fullPath);    // 경로 저장
            m_SaveFileNames.push_back(fileName); // 이름 저장
        }
           
    }



    for (auto& str : m_SaveFileNames)
        m_SaveFileNamesStr.push_back(str.c_str());

    
        

    return S_OK;
}

void CSaveWindow::Show_ListBox()
{
    if (ImGui::ListBox("SaveFileList", &LoadFileIdx, m_SaveFileNamesStr.data(), m_SaveFileNamesStr.size()))
    {
        pGameInstance->Load_Terrains(m_SaveFiles[LoadFileIdx]);
        pMapObject_Manager->Set_SelectObject(nullptr);


    }

}

CSaveWindow* CSaveWindow::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg)
{
    CSaveWindow* pInstance = new CSaveWindow(pDevice, pContext);


    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Create : CSaveWindow");
        Safe_Release(pInstance);
    }


    return pInstance;
}

void CSaveWindow::Free()
{ 
    __super::Free();

    Safe_Release(pGameInstance);
    Safe_Release(pMapObject_Manager);
    Safe_Release(m_pSaveButton);
}

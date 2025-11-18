#include "CMenuBarWindow.h"
#include "CGameInstance.h"
#include "CImgui_DataManager.h"
#include "CMapObject_Manager.h"
#include "CImGui_Manager.h"
#include "CMapObject_Manager.h"
#include "CNavMeshEdit_Manager.h"

USING(MapTool)

CMenuBarWindow::CMenuBarWindow(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CImgui_Window(pDevice,pContext),
    pGameInstance(CGameInstance::GetInstance()),
    m_pMapObject_Manager(CMapObject_Manager::GetInstance())
    , m_pImgui_DataManager(CImgui_DataManager::GetInstance())
{
    Safe_AddRef(pGameInstance);
    Safe_AddRef(m_pImgui_DataManager);

}

HRESULT CMenuBarWindow::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;


    pGameInstance->Register_HotKey(KeyCode::S, true, false, false, [&]()
        {
			if (FAILED(CGameInstance::GetInstance()->Save_All_Terrains(m_SaveFilePath.m_CurrentTerrainSaveFilePath, m_SaveFilePath.m_TerrainSaveFiles.size())))
				return;

            //Refresh SaveFileList
            if (FAILED(m_pImgui_DataManager->Update_SaveFiles()))
                return;
        });


    pGameInstance->Register_HotKey(KeyCode::S, true, true, false, [&]()
        {
            m_bTextOpen = true;
        });

	return S_OK;
}

void CMenuBarWindow::Update_Priority()
{
    __super::Update_Priority();
    if (m_bLoad)
    {
        CMapObject_Manager::GetInstance()->Set_SelectObject(nullptr);

   
        m_bLoad = false;

    }
}

void CMenuBarWindow::Update()
{
    

    Update_Menu();

   

}

void CMenuBarWindow::Render()
{
}

CMenuBarWindow* CMenuBarWindow::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg)
{
    CMenuBarWindow* pInstance = new CMenuBarWindow(pDevice, pContext);


    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Create : CMenuBarWindow");
        Safe_Release(pInstance);
    }


    return pInstance;
}


void CMenuBarWindow::Update_Menu()
{
    m_SaveFilePath = m_pImgui_DataManager->Get_SaveFilePath();
    m_LoadFilePath = m_pImgui_DataManager->Get_LoadFilePath();

 

    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            Show_SaveMenu();
            Show_SaveAsMenu();
            Show_SaveAllMenu();


            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Load"))
        {
            Show_LoadMenu();

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Mode"))
        {
            Show_ModeMenu();
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

    /// <summary>
    /// 메뉴클릭에따라 활성화/비활성화된다.
    /// </summary>

    Show_TerrainListBox();
    Show_NavListBox();
    Show_TextBox();
}

void CMenuBarWindow::Show_SaveMenu()
{
    //그냥 일반 저장, 불러오기했으면 덮어씌워져야하고, 아니면 그냥 연속적ㅇ파일로 저장
   
    if (ImGui::BeginMenu("Save", "Ctrl+S"))
    {
#pragma region Save_Terrain
        if (ImGui::MenuItem("Save_Terrain","Ctrl+S"))
        {
            //눌렀을떄.
            if (m_pImgui_DataManager->IsLoadedTerrain())
            {

                //덮어쓰기
                //저장함수호출
                //obj.저장
                //terrain저장
                if (FAILED(CGameInstance::GetInstance()->Save_All_Terrains(m_LoadFilePath.m_CurrentLoadTerrainFilePath, -1)))
                    return;

                //다시초기화..
                m_LoadFilePath.m_CurrentLoadTerrainFilePath = "";
                m_pImgui_DataManager->Set_LoadFilePath(m_LoadFilePath);

            }


            else  //그냥일반저장
            {
                if (FAILED(CGameInstance::GetInstance()->Save_All_Terrains(m_SaveFilePath.m_CurrentTerrainSaveFilePath, m_SaveFilePath.m_TerrainSaveFiles.size())))
                    return;

            }


            //Refresh SaveFileList
            if (FAILED(m_pImgui_DataManager->Update_SaveFiles()))
                return;
       }

#pragma endregion
        if (ImGui::MenuItem("Save_NavMesh","Ctrl+S"))
        {
            if (m_pImgui_DataManager->IsLoadedNav())
            {
                if (FAILED(CNavMeshEdit_Manager::GetInstance()->Save_NavigationData(m_SaveFilePath.m_CurrentNavSaveFilePath)))
                {
                    MSG_BOX("Nothing to Save!");
                    return;
                }

                m_LoadFilePath.m_CurrentLoadNavFilePath = "";
                m_pImgui_DataManager->Set_LoadFilePath(m_LoadFilePath);
            }

            //Refresh SaveFileList
            if (FAILED(m_pImgui_DataManager->Update_SaveFiles()))
                return;
        }

     

        ImGui::EndMenu();
    }

}

void CMenuBarWindow::Show_SaveAsMenu()
{
    //무조건 다른이름으로 저장
    if(ImGui::MenuItem("Save As", "Ctrl+Shift+S"))
    {
        m_bTextOpen = true;
    }
}

void CMenuBarWindow::Show_SaveAllMenu()
{
    //terrain이름으로 저장
    if (ImGui::MenuItem("Save All", "Ctrl+S+A"))
    {

        if (FAILED(CGameInstance::GetInstance()->Save_All_Terrains(m_SaveFilePath.m_CurrentTerrainSaveFilePath, m_SaveFilePath.m_TerrainSaveFiles.size())))
            return;

        fs::path NavFilePath = m_SaveFilePath.m_CurrentTerrainSaveFilePath;
        string FileName = NavFilePath.filename().stem().string();       //Terrain7

        CNavMeshEdit_Manager::GetInstance()->Save_NavigationData(m_SaveFilePath.m_SavePathBase+FileName+"_Nav.dat");

        //Refresh SaveFileList
        if (FAILED(m_pImgui_DataManager->Update_SaveFiles()))
            return;
    }
 

}

void CMenuBarWindow::Show_LoadMenu()
{
    if (ImGui::BeginMenu("Load", "Ctrl+L"))
    {
        if (ImGui::MenuItem("Load_Terrain", "Ctrl+L"))
        {
            //리스트보여줘야함.
            m_bTerrainListOpen = true;


        }

        if (ImGui::MenuItem("Load_NavMesh", "Ctrl+L"))
        {
            m_bNavListOpen = true;

          
        }


        ImGui::EndMenu();
    }
}

void CMenuBarWindow::Show_ModeMenu()
{
    if (ImGui::MenuItem("NavMeshMode", "Ctrl+N"))
    {
        CImGui_Manager::GetInstance()->Set_MapToolMode(MapToolMode::NAVMESH);
        CMapObject_Manager::GetInstance()->Set_SelectObject(nullptr);
    }

    if (ImGui::MenuItem("EditMode", "Ctrl+E"))
    {
        CImGui_Manager::GetInstance()->Set_MapToolMode(MapToolMode::EDIT);
        CMapObject_Manager::GetInstance()->Set_SelectObject(nullptr);
    }
}

void CMenuBarWindow::Show_TerrainListBox()
{
    /*리스트에 표시할 이름배열 만들어*/
   
    CheckFalse(m_bTerrainListOpen);

    ImGui::Begin("TerrainFileList", &m_bTerrainListOpen);


    if (ImGui::ListBox("TerrainFileList", &m_LoadFilePath.LoadTerarinFileIdx, m_SaveFilePath.m_TerrainSaveFileNamesStr.data(), m_SaveFilePath.m_TerrainSaveFileNamesStr.size()))
    {
        m_bLoad = true;
        CMapObject_Manager::GetInstance()->Set_SelectObject(nullptr);

        //불러오기 및 덮어쓰기를위한 경로갱신
        if (FAILED(pGameInstance->Load_Terrains_MapTool(m_SaveFilePath.m_TerrainSaveFiles[m_LoadFilePath.LoadTerarinFileIdx])))
            return;

        m_LoadFilePath.m_CurrentLoadTerrainFilePath = m_SaveFilePath.m_TerrainSaveFiles[m_LoadFilePath.LoadTerarinFileIdx];
        m_pImgui_DataManager->Set_LoadFilePath(m_LoadFilePath);

      
    }

    ImGui::End();
}

void CMenuBarWindow::Show_NavListBox()
{
    CheckFalse(m_bNavListOpen);

    ImGui::Begin("NavFileList", &m_bNavListOpen); 


    if (ImGui::ListBox("NavFileList", &m_LoadFilePath.LoadNavFileIdx, m_SaveFilePath.m_NavSaveFileNamesStr.data(), m_SaveFilePath.m_NavSaveFileNamesStr.size()))
    {
        m_bLoad = true;


        CMapObject_Manager::GetInstance()->Set_SelectObject(nullptr);

        //불러오기 및 덮어쓰기를위한 경로갱신
        if (FAILED(CNavMeshEdit_Manager::GetInstance()->Load_NavigationData(m_SaveFilePath.m_NavSaveFiles[m_LoadFilePath.LoadNavFileIdx])))
        {
            ImGui::End();
            MSG_BOX("Nothing to Load!, Empty");
            return;
        }
        m_LoadFilePath.m_CurrentLoadNavFilePath = m_SaveFilePath.m_NavSaveFiles[m_LoadFilePath.LoadNavFileIdx];
        m_pImgui_DataManager->Set_LoadFilePath(m_LoadFilePath);

      
    }

    ImGui::End();
}

void CMenuBarWindow::Show_TextBox()
{
    CheckFalse(m_bTextOpen);

    if (ImGui::InputText("Save Name", m_szSaveName, MAX_PATH, ImGuiInputTextFlags_EnterReturnsTrue))
    {
        //내가작성한 이름으로 저장할꺼야.
        m_SaveFilePath.m_CurrentTerrainSaveFilePath = m_SaveFilePath.m_SavePathBase + m_szSaveName + ".json";

        if (FAILED(CGameInstance::GetInstance()->Save_All_Terrains(m_SaveFilePath.m_CurrentTerrainSaveFilePath, m_SaveFilePath.m_TerrainSaveFiles.size())))
            return;

        //Refresh SaveFileList
        if (FAILED(m_pImgui_DataManager->Update_SaveFiles()))
            return;

        m_bTextOpen = false;
    }
}




void CMenuBarWindow::Free()
{
    Safe_Release(pGameInstance);
    Safe_Release(m_pMapObject_Manager);
    Safe_Release(m_pImgui_DataManager);
}


#include "CMenuBarWindow.h"
#include "CGameInstance.h"
#include "CImgui_DataManager.h"
#include "CMapObject_Manager.h"
USING(MapTool)

CMenuBarWindow::CMenuBarWindow(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CImgui_Window(pDevice,pContext),
    pGameInstance(CGameInstance::GetInstance()),
    m_pMapObject_Manager(CMapObject_Manager::GetInstance())
    , m_pImgui_DataManager(CImgui_DataManager::GetInstance())
{
    Safe_AddRef(pGameInstance);
    Safe_AddRef(m_pMapObject_Manager);
    Safe_AddRef(m_pImgui_DataManager);

}

HRESULT CMenuBarWindow::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;


    pGameInstance->Register_HotKey(KeyCode::S, true, false, false, [&]()
        {
			if (FAILED(CGameInstance::GetInstance()->Save_All_Terrains(m_SaveFilePath.m_CurrentSaveFilePath, m_SaveFilePath.m_SaveFiles.size())))
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


            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Load"))
        {
            Show_LoadMenu();

            ImGui::EndMenu();
        }


        ImGui::EndMainMenuBar();
    }

    /// <summary>
    /// 메뉴클릭에따라 활성화/비활성화된다.
    /// </summary>
    Show_ListBox();
    Show_TextBox();
}

void CMenuBarWindow::Show_SaveMenu()
{
    //그냥 일반 저장, 불러오기했으면 덮어씌워져야하고, 아니면 그냥 연속적ㅇ파일로 저장
    if (ImGui::MenuItem("Save", "Ctrl+S"))
    {
       
        //눌렀을떄.
        if (m_pImgui_DataManager->IsLoaded())
        {

            //덮어쓰기
            //저장함수호출
            //obj.저장
            //terrain저장
            if (FAILED(CGameInstance::GetInstance()->Save_All_Terrains(m_LoadFilePath.m_CurrentLoadFilePath, -1)))
                return;

            //다시초기화..
            m_LoadFilePath.m_CurrentLoadFilePath = "";
            m_pImgui_DataManager->Set_LoadFilePath(m_LoadFilePath);
            
        }


        else  //그냥일반저장
        {
            if (FAILED(CGameInstance::GetInstance()->Save_All_Terrains(m_SaveFilePath.m_CurrentSaveFilePath, m_SaveFilePath.m_SaveFiles.size())))
                return;

        }


        //Refresh SaveFileList
        if (FAILED(m_pImgui_DataManager->Update_SaveFiles()))
            return;
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

void CMenuBarWindow::Show_LoadMenu()
{
    if (ImGui::MenuItem("Load", "Ctrl+L"))
    {
        //리스트보여줘야함.
        m_bListOpen = true;
    

       

         


    }
}

void CMenuBarWindow::Show_ListBox()
{
    /*리스트에 표시할 이름배열 만들어*/
   
    CheckFalse(m_bListOpen);

    ImGui::Begin("SaveFileList", &m_bListOpen);


    if (ImGui::ListBox("SaveFileList", &m_LoadFilePath.LoadFileIdx, m_SaveFilePath.m_SaveFileNamesStr.data(), m_SaveFilePath.m_SaveFileNamesStr.size()))
    {
        m_pMapObject_Manager->Set_SelectObject(nullptr);

        //불러오기 및 덮어쓰기를위한 경로갱신
        if (FAILED(pGameInstance->Load_Terrains_MapTool(m_SaveFilePath.m_SaveFiles[m_LoadFilePath.LoadFileIdx])))
            return;

        m_LoadFilePath.m_CurrentLoadFilePath = m_SaveFilePath.m_SaveFiles[m_LoadFilePath.LoadFileIdx];
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
        m_SaveFilePath.m_CurrentSaveFilePath = m_SaveFilePath.m_SavePathBase + m_szSaveName + ".json";

        if (FAILED(CGameInstance::GetInstance()->Save_All_Terrains(m_SaveFilePath.m_CurrentSaveFilePath, m_SaveFilePath.m_SaveFiles.size())))
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


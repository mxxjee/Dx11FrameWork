#include "CLevelDebugWindow.h"
#include "CLevel_Manager.h"
#include "CGameInstance.h"
#include "CLevel.h"
#include "Client_Defines.h"
#include "CLayer.h"
#include "CGameObject.h"
#include "Engine_Function.h"
#include "CImGui_Manager.h"
#include "CObjectDebugWindow.h"


USING(Client)

CLevelDebugWindow::CLevelDebugWindow(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CImgui_Window(pDevice,pContext),
    pGameInstance(CGameInstance::GetInstance())
{
    Safe_AddRef(pGameInstance);
}

HRESULT CLevelDebugWindow::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

void CLevelDebugWindow::Update()
{

    ImGui::Begin(m_WindowTitle.c_str(), &m_bOpen);
    
    const auto& stack = pGameInstance->Get_LevelStack();
    if (stack->empty())
    {
        ImGui::Text("Scene Stack is Empty!!");
    }

    else
    {
        
        //스택 순회(아래->위)
        for (int i = stack->size()-1; i >=0; --i)
        {
            CLevel* pLevel = (*stack)[i];
            if (pLevel)
            {
                string LevelName = "";
                switch (static_cast<LEVEL_ID>(pLevel->Get_LevelID()))
                {
                case LEVEL_ID::STATIC:
                    LevelName = "STATIC";
                    break;

                case LEVEL_ID::LOGO:
                    LevelName = "LOGO";
                    break;

                case LEVEL_ID::LOADING:
                    LevelName = "LOADING";
                    break;

                case LEVEL_ID::TOWN:
                    LevelName = "TOWN";
                    break;

                case LEVEL_ID::UI:
                    LevelName = "UI";
                    break;
                }

                if (ImGui::CollapsingHeader(LevelName.c_str()))
                {
                    auto& Layers = pGameInstance->Get_Layers(pLevel->Get_LevelID());
                    for (auto& pair : Layers)
                    {
                        const _wstring LayerTag = pair.first;
                        CLayer* pLayer = pair.second;
                        int count = static_cast<int>(pLayer->Get_ObjList().size());
                        string name = WStringToUTF8(LayerTag);

                        char label[256];
                        sprintf_s(label, "%s   /  count: %d##%s", name.c_str(), count, name.c_str());

                        //현재 씬의 레이어/오브젝트 이름 표시
                        if (pLayer)
                        {
                            if (ImGui::TreeNode(label))
                            {
                                for (auto& i : pLayer->Get_ObjList())
                                {
                                    //각 오브젝트 표시 UI 선택가능
                                    if (ImGui::Selectable(WStringToUTF8(i->Get_Tag()).c_str(), pSelectObject ==i))
                                    {
                                        CImgui_Base* pBase = CImGui_Manager::GetInstance()->Find_Window("ObjectDebugWindow");
                                        if (pBase)
                                        {
                                            CObjectDebugWindow* pWindow = dynamic_cast<CObjectDebugWindow*>(pBase);
                                            if (pWindow)
                                                pWindow->Set_SelectObject(i);
                                        }
                                    }
                                }
                                  
                                ImGui::TreePop();
                            }
                            
                        }

                    }
                }
            }
        }

        /*Static 씬 꺼 표시*/
        if (ImGui::CollapsingHeader("STATIC"))
        {
            auto& Layers = pGameInstance->Get_Layers(ENUM_TO_UINT(LEVEL_ID::STATIC));
            for (auto& pair : Layers)
            {
                const _wstring LayerTag = pair.first;
                CLayer* pLayer = pair.second;

                string name = WStringToUTF8(LayerTag);
                int count = static_cast<int>(pLayer->Get_ObjList().size());

                char label[256];
                sprintf_s(label, "%s   /  count: %d##%s", name.c_str(), count, name.c_str());
                //현재 씬의 레이어/오브젝트 이름 표시
                if (pLayer)
                {
                    if (ImGui::TreeNode(label))
                    {
                        for (auto& i : pLayer->Get_ObjList())
                        {
                            //각 오브젝트 표시 UI 선택가능
                            if (ImGui::Selectable(WStringToUTF8(i->Get_Tag()).c_str(), pSelectObject == i))
                            {
                                CImgui_Base* pBase = CImGui_Manager::GetInstance()->Find_Window("ObjectDebugWindow");
                                if (pBase)
                                {
                                    CObjectDebugWindow* pWindow = dynamic_cast<CObjectDebugWindow*>(pBase);
                                    if (pWindow)
                                        pWindow->Set_SelectObject(i);
                                }
                            }
                        }

                        ImGui::TreePop();
                    }
                   
                }

            }
        }
       
    }
    ImGui::End();

 
}

void CLevelDebugWindow::Render()
{
   
}

CLevelDebugWindow* CLevelDebugWindow::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg)
{
    CLevelDebugWindow* pInstance = new CLevelDebugWindow(pDevice, pContext);


    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Create : LevelDebugWindow");
        Safe_Release(pInstance);
    }


    return pInstance;
}

void CLevelDebugWindow::Free()
{
    __super::Free();
    Safe_Release(pGameInstance);
}
 
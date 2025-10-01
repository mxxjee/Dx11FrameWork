#include "CLevelDebugWindow.h"
#include "CLevel_Manager.h"
#include "CGameInstance.h"
#include "CLevel.h"
#include "Client_Defines.h"
#include "CLayer.h"
#include "CGameObject.h"


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

                case LEVEL_ID::GAMEPLAY:
                    LevelName = "GAMEPLAY";
                    break;
                }

                if (ImGui::CollapsingHeader(LevelName.c_str()))
                {
                    auto& Layers = pGameInstance->Get_Layers(pLevel->Get_LevelID());
                    for (auto& pair : Layers)
                    {
                        CLayer* pLayer = pair.second;
                        if (pLayer)
                        {
                            for (auto& i : pLayer->Get_ObjList())
                                ImGui::Text("%S", i->Get_Tag().c_str());
                        }

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
    Safe_Release(pGameInstance);
}
 
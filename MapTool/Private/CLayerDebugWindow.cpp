#include "CLayerDebugWindow.h"
#include "CGameInstance.h"

#include "CMapLayer.h"
#include "CMapObject.h"

#include "CObjectInspectorWindow.h"
#include "CImGui_Manager.h"
#include "CMapObject_Manager.h"


USING(MapTool)
CLayerDebugWindow::CLayerDebugWindow(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CImgui_Window(pDevice, pContext),
    pGameInstance(CGameInstance::GetInstance()), m_pMapObject_Manager(CMapObject_Manager::GetInstance())
{
    Safe_AddRef(m_pMapObject_Manager);
    Safe_AddRef(pGameInstance);
}

HRESULT CLayerDebugWindow::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

void CLayerDebugWindow::Update()
{
    ImGui::Begin(m_WindowTitle.c_str(), &m_bOpen);
    
    pSelectObject = m_pMapObject_Manager->Get_SelectObject();
    //레이어목록을 표시
    UMap<_wstring, CMapLayer*> m_Layers = m_pMapObject_Manager->Get_MapLayers();
    for (auto& pair : m_Layers)
    {
        if (pair.second)
        {
            const _wstring LayerTag = pair.first;
            CMapLayer* pLayer = pair.second;

            string name = WStringToUTF8(LayerTag);
            int count = pLayer->Get_ObjList().size();

            char label[256];
            sprintf_s(label, "%s   /  count: %d##%s", name.c_str(), count, name.c_str());
            if (ImGui::TreeNode(label))
            {
                for (auto& i : pLayer->Get_ObjList())
                {
                    bool bSelected = (pSelectObject == i); // 현재 선택된 상태인지
                    if (ImGui::Selectable(WStringToUTF8(i->Get_Tag()).c_str(), bSelected))
                    {
                        m_pMapObject_Manager->Set_SelectObject(i);
                        
                    }
                }

                ImGui::TreePop();
            }
        }
    }
    ImGui::End();
}

void CLayerDebugWindow::Render()
{
}

CLayerDebugWindow* CLayerDebugWindow::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg)
{
    CLayerDebugWindow* pInstance = new CLayerDebugWindow(pDevice, pContext);


    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Create : CLayerDebugWindow");
        Safe_Release(pInstance);
    }


    return pInstance;
}

void CLayerDebugWindow::Free()
{
    __super::Free();
    Safe_Release(m_pMapObject_Manager);
    Safe_Release(pGameInstance);
}

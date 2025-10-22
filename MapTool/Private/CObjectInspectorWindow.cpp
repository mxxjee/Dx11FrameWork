#include "CObjectInspectorWindow.h"
#include "CGameInstance.h"
#include "CMapObject.h"
USING(MapTool)
CObjectInspectorWindow::CObjectInspectorWindow(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CImgui_Window(pDevice,pContext),
    pGameInstance(CGameInstance::GetInstance())
{
    Safe_AddRef(pGameInstance);
}

HRESULT CObjectInspectorWindow::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

void CObjectInspectorWindow::Update()
{
    ImGui::Begin(m_WindowTitle.c_str(), &m_bOpen);
    ImGui::End();
}

void CObjectInspectorWindow::Render()
{
}

CObjectInspectorWindow* CObjectInspectorWindow::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg)
{
    CObjectInspectorWindow* pInstance = new CObjectInspectorWindow(pDevice, pContext);


    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Create : CObjectInspectorWindow");
        Safe_Release(pInstance);
    }


    return pInstance;
}

void CObjectInspectorWindow::Free()
{
    __super::Free();
    Safe_Release(pGameInstance);
}

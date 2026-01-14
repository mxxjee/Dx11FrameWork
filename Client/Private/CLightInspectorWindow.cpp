#include "CLightInspectorWindow.h"
#include "CLight.h"

USING(Client)
CLightInspectorWindow::CLightInspectorWindow(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CImgui_Window(pDevice,pContext)
{
}

HRESULT CLightInspectorWindow::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;





    return S_OK;
}

void CLightInspectorWindow::Update()
{

    ImGui::Begin(m_WindowTitle.c_str(), &m_bOpen);

    if (pSelectObject)
    {
        pSelectObject->Render_DebugImgui();

    }




    ImGui::End();

}

void CLightInspectorWindow::Render()
{
}

void CLightInspectorWindow::Set_SelectObject(CLight* pTarget)
{
    if (pTarget != pSelectObject)
    {
        pSelectObject = pTarget;

    }
}

CLightInspectorWindow* CLightInspectorWindow::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg)
{
    CLightInspectorWindow* pInstance = new CLightInspectorWindow(pDevice, pContext);


    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Create : CLightInspectorWindow");
        Safe_Release(pInstance);
    }


    return pInstance;
}

void CLightInspectorWindow::Free()
{
    __super::Free();
}

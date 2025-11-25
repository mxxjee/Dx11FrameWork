#include "CStateDebugWindow.h"
#include "CGameObject.h"
#include "CPlayer.h"
#include "CImgui_Checkbox.h"


USING(Client)
CStateDebugWindow::CStateDebugWindow(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CImgui_Window(pDevice,pContext)
{
}

HRESULT CStateDebugWindow::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    return S_OK;
}

void CStateDebugWindow::Update()
{
    if (m_pPlayer)
        pActionControl = m_pPlayer->Get_ActionControl();

    CheckNull(pActionControl);

    

    ImGui::Begin(m_WindowTitle.c_str(), &m_bOpen);
    if (pSelectObject)
    {
        CModelObject* pModel = dynamic_cast<CModelObject*>(pSelectObject);
        if (pModel)
            pModel->Render_StateDebug(&Value);

    }

  

    ImGui::End();
}

void CStateDebugWindow::Render()
{
}



CStateDebugWindow* CStateDebugWindow::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg)
{
    CStateDebugWindow* pInstance = new CStateDebugWindow(pDevice, pContext);


    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Create : CStateDebugWindow");
        Safe_Release(pInstance);
    }


    return pInstance;
}

void CStateDebugWindow::Free()
{
}

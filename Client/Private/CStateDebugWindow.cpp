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

    if (ImGui::RadioButton("Ladder", (int*)(&Value), 0))
    {
        pActionControl->m_bLadder = true;
        pActionControl->m_bPush = false;
        pActionControl->m_bCarry = false;
        pActionControl->m_bItemGet = false;
    }
    if (ImGui::RadioButton("Push", (int*)(&Value), 1))
    {
        pActionControl->m_bLadder = false;
        pActionControl->m_bPush = true;
        pActionControl->m_bCarry = false;
        pActionControl->m_bItemGet = false;
    }


    if (ImGui::RadioButton("Carry", (int*)(&Value), 2))
    {
        pActionControl->m_bLadder = false;
        pActionControl->m_bPush = false;
        pActionControl->m_bCarry = true;
        pActionControl->m_bItemGet = false;
    }


    if (ImGui::RadioButton("ItemGet", (int*)(&Value), 3))
    {
        pActionControl->m_bLadder = false;
        pActionControl->m_bPush = false;
        pActionControl->m_bCarry = false;
        pActionControl->m_bItemGet = true;
    }

    if (ImGui::RadioButton("None", (int*)(&Value),4))
    {
        pActionControl->m_bLadder = false;
        pActionControl->m_bPush = false;
        pActionControl->m_bCarry = false;
        pActionControl->m_bItemGet = false;
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

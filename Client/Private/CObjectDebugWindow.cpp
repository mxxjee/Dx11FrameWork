#include "CObjectDebugWindow.h"
#include "CGameObject.h"
#include "CTransform.h"
#include "MathUtils.h"
#include "ColorUtils.h"
#include "CModelObject.h"
#include "CImGui_Manager.h"
#include "CAnimModelObject.h"

USING(Client)
CObjectDebugWindow::CObjectDebugWindow(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CImgui_Window(pDevice,pContext)
{
}

HRESULT CObjectDebugWindow::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;





    return S_OK;
}

void CObjectDebugWindow::Update()
{
#ifdef _DEBUG
    ImGui::Begin(m_WindowTitle.c_str(), &m_bOpen);

    if (pSelectObject)
    {
        pSelectObject->Render_Transform_Imgui();

        if (pSelectModelObj)
        {
            if(pSelectAnimModelObj)
                pSelectAnimModelObj->Render_CurrentState_Animation();
        }
    }

        

   
    

    ImGui::End();
 
#endif
}

void CObjectDebugWindow::Render()
{
}

void CObjectDebugWindow::Set_SelectObject(CGameObject* pTarget)
{
    if (pTarget != pSelectObject)
    {
        pSelectObject = pTarget;

        pSelectModelObj = dynamic_cast<CModelObject*>(pSelectObject);
        pSelectAnimModelObj = dynamic_cast<CAnimModelObject*>(pSelectModelObj);
    }
}

CObjectDebugWindow* CObjectDebugWindow::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg)
{
    CObjectDebugWindow* pInstance = new CObjectDebugWindow(pDevice, pContext);


    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Create : ObjectDebugWindow");
        Safe_Release(pInstance);
    }


    return pInstance;
}

void CObjectDebugWindow::Free()
{
    __super::Free();

}

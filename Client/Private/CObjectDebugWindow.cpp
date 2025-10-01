#include "CObjectDebugWindow.h"
#include "CGameObject.h"
#include "CTransform.h"

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
    ImGui::Begin(m_WindowTitle.c_str(), &m_bOpen);

    if (pSelectObject)
    {
        ImGui::Text("Name : %s", WStringToUTF8(pSelectObject->Get_Tag()).c_str());


        CTransform* pTransform = dynamic_cast<CTransform*>(pSelectObject->Get_Component(L"Transform"));
        if (pTransform)
        {
            ImGui::Separator();

            _float3 pos,scale;
            XMStoreFloat3(&pos, pTransform->Get_State(STATE::POSITION));
            scale=pTransform->Get_Scale();
            
            ImGui::BulletText("Position X:%f, Y:%f, Z:%f", pos.x, pos.y, pos.z);
            ImGui::Separator();
            ImGui::BulletText("Scale X:%f, Y:%f, Z:%f", scale.x, scale.y, scale.z);
        }
    }
        

   
    

    ImGui::End();

}

void CObjectDebugWindow::Render()
{
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

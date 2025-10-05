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

            _vector Scale, Trans, Rotation;
            _float3 s, t, r;

            
            XMMatrixDecompose(&Scale,  &Rotation, &Trans, XMLoadFloat4x4(&pTransform->Get_World()));
            XMStoreFloat3(&s, Scale);
            XMStoreFloat3(&t, Trans);
            XMStoreFloat3(&r, Rotation);

            ImGui::BulletText("Position X:%f, Y:%f, Z:%f", t.x, t.y, t.z);
            ImGui::Separator();
            ImGui::BulletText("Scale X:%f, Y:%f, Z:%f", s.x, s.y, s.z);
            ImGui::Separator();
            ImGui::BulletText("Rotation X:%f, Y:%f, Z:%f", r.x, r.y, r.z);
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

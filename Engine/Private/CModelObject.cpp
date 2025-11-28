#include "CModelObject.h"
#include "CGameInstance.h"
#include "CModel.h"
#include "CShader.h"
#include "CInput_Manager.h"
#include "CMeshComponent.h"
#include "CModel.h"
#include "CBody.h"
#include "CAnimation.h"




CModelObject::CModelObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CContainerObject(pDevice,pContext)
{

}

CModelObject::CModelObject(const CModelObject& rhs)
    : CContainerObject(rhs)

{

}

HRESULT CModelObject::Initialize_Prototype()
{

    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CModelObject::Initialize_Copytype(void* pArg)
{
    /*부모 컴포넌트 값세팅 */
    if (FAILED(__super::Initialize_Copytype(pArg)))
        return E_FAIL;

    /*내 컴포넌트 값 세팅*/
    if (FAILED(Ready_Components(pArg)))
        return E_FAIL;

  
 
  
    return S_OK;

}

void CModelObject::Update_Priority(_float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
}

void CModelObject::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
    CheckNull(m_pTransformCom);

}

void CModelObject::Update_Late(_float fTimeDelta)
{

    __super::Update_Late(fTimeDelta);
}

void CModelObject::Update_Render(_float fTimeDelta)
{
    __super::Update_Render(fTimeDelta);

}

HRESULT CModelObject::Render()
{
  

    return S_OK;
}



bool CModelObject::Is_AnimEnd()
{
    CheckNullResult(m_pBody, false);
    return m_pBody->Get_IsAnimFinished();
}

HRESULT CModelObject::Bind_ShaderResources()
{
   

    return S_OK;
}



CModelObject* CModelObject::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
{
    CModelObject* pInstance = new CModelObject(_pDevice, _pDeviceContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create :CModelObject ");
        Safe_Release(pInstance);

    }



    return pInstance;
}

CGameObject* CModelObject::Clone(void* pArg)
{
    CModelObject* pInstance = new CModelObject(*this);
    if (FAILED(pInstance->Initialize_Copytype(pArg)))
    {
        MSG_BOX("Failed to Cloned :CModelObject ");
        Safe_Release(pInstance);

    }
    return pInstance;
}


HRESULT CModelObject::Ready_Components(void* pArg)
{
   
    return S_OK;
}


HRESULT CModelObject::Ready_PartObjects(void* pArg)
{
    CheckNullResult(pArg, E_FAIL);
    MODELOBJECT_DESC* pModelDesc = static_cast<MODELOBJECT_DESC*>(pArg);
    if (pModelDesc)
    {
        CBody::BODY_DESC* pBodyDesc = static_cast<CBody::BODY_DESC*>(pModelDesc->BodyDesc);
        if (FAILED(__super::Add_PartObject(0, PROTO_OBJ_NAME(L"Body"), L"Part_Body", pBodyDesc)))
            return E_FAIL;

    }
    return S_OK;

}
void CModelObject::Render_CurrentState_Animation()
{
    ImGui::Separator();
    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255));

    string pPreState = Convert_String_To_Enum(m_iPreState);
    string pCurState = Convert_String_To_Enum(m_iState);

    ImGui::BulletText("PreState:%s",
        pPreState.c_str());

    ImGui::BulletText("CurrentState:%s",
        pCurState.c_str());



    CAnimation* pCurrentAnim = m_pBody->Get_Model()->Get_CurrentAnim();
    string CurrentAnimKey = WStringToUTF8(m_pBody->Get_Model()->Get_CurrentAnimKey());
    
   

    ImGui::BulletText("NowPlaying:%s",
        CurrentAnimKey.c_str());


    ImGui::BulletText("Now KeyFrame:%d",
        pCurrentAnim->Get_CurrentKeyFrameIndex(0));
    ImGui::PopStyleColor();
}
string CModelObject::Convert_String_To_Enum(_uint eState)
{
    string StateDebugStr="";

    if (eState == 0)
        return "NONE";
        

    else
    {
        if (eState & IDLE)
            StateDebugStr += "IDLE ";

        if (eState & RUN)
            StateDebugStr += "Run ";

        if (eState & ATTACK)
            StateDebugStr += "ATTACK ";
    }
   


    return StateDebugStr;
}
void CModelObject::Free()
{
    __super::Free();

}


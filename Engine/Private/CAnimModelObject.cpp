#include "CAnimModelObject.h"
#include "CBody.h"
#include "CAnimation.h"
#include "CModel.h"
#include "CAnimBody.h"


CAnimModelObject::CAnimModelObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CModelObject(pDevice, pContext)
{
}

CAnimModelObject::CAnimModelObject(const CAnimModelObject& rhs)
    : CModelObject(rhs)
{
}

HRESULT CAnimModelObject::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CAnimModelObject::Initialize_Copytype(void* pArg)
{
    if (FAILED(__super::Initialize_Copytype(pArg)))
        return E_FAIL;

    return S_OK;
}

void CAnimModelObject::Update_Priority(_float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
}

void CAnimModelObject::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CAnimModelObject::Update_Late(_float fTimeDelta)
{

    __super::Update_Late(fTimeDelta);
}

void CAnimModelObject::Update_Render(_float fTimeDelta)
{
    __super::Update_Render(fTimeDelta);
}

HRESULT CAnimModelObject::Render()
{
    return S_OK;
}

bool CAnimModelObject::Is_AnimEnd()
{
    CheckNullResult(m_pAnimBody, false);
    return m_pAnimBody->Get_IsAnimFinished();
}

#ifdef _DEBUG
void CAnimModelObject::Render_DebugImgui()
{
    __super::Render_DebugImgui();
    Render_CurrentState_Animation();
}
void CAnimModelObject::Render_CurrentState_Animation()
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
        pCurrentAnim->Get_CurrentFrame(pCurrentAnim->Get_CurrentTrackPoistion()));
    ImGui::PopStyleColor();
}

string CAnimModelObject::Convert_String_To_Enum(_uint eState)
{
    string StateDebugStr = "";

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

#endif
CAnimModelObject* CAnimModelObject::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
{
    CAnimModelObject* pInstance = new CAnimModelObject(_pDevice, _pDeviceContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create :CAnimModelObject ");
        Safe_Release(pInstance);

    }



    return pInstance;
}
CGameObject* CAnimModelObject::Clone(void* pArg)
{
    CGameObject* pInstance = new CAnimModelObject(*this);
    if (FAILED(pInstance->Initialize_Copytype(pArg)))
    {
        MSG_BOX("Failed to Cloned :CAnimModelObject ");
        Safe_Release(pInstance);

    }
    return pInstance;
}
void CAnimModelObject::Free()
{
    __super::Free();
}
CBody* CAnimModelObject::Get_Body()
{
    return m_pAnimBody;
}

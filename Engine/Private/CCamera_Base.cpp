#include "CCamera_Base.h"
#include "CGameInstance.h"
#include "CCameraComponent.h"
#include "CShader.h"


CCamera_Base::CCamera_Base(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CGameObject(pDevice,pContext)
{
}

CCamera_Base::CCamera_Base(const CCamera_Base& rhs)
    :CGameObject(rhs),m_pMainShader(rhs.m_pMainShader),m_pCameraCom(rhs.m_pCameraCom), m_GlobalViewProj(rhs.m_GlobalViewProj)
{
}

HRESULT CCamera_Base::Initialize_Prototype()
{
    
    return S_OK;
}

HRESULT CCamera_Base::Initialize_Copytype(void* pArg)
{
    if(FAILED(__super::Initialize_Copytype(pArg)))
        return E_FAIL;

    m_pMainShader = m_pGameInstance->Find_Shader(L"Default");
    m_GlobalViewProj = m_pMainShader->Get_ShaderInfo().m_GlobalViewProj;

    return S_OK;
}

void CCamera_Base::Free()
{
    __super::Free();
    Safe_Release(m_pCameraCom);
}

void CCamera_Base::Bind_ViewProjMatrix()
{
    //카메라 view/투영 세팅
    if (m_pMainShader)
    {
        _float4x4 viewproj;
        XMStoreFloat4x4(&viewproj, m_pCameraCom->Get_MulViewProjMatrix());
        m_GlobalViewProj->AsMatrix()->SetMatrix((float*)viewproj.m);
    }
}



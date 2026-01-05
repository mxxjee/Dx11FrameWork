#include "CMeshEffect_RollCut.h"
#include "CShader.h"
#include "MathUtils.h"
#include "CTransform.h"


USING(Client)

CMeshEffect_RollCut::CMeshEffect_RollCut(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CMeshEffect(pDevice, pContext)
{
}

CMeshEffect_RollCut::CMeshEffect_RollCut(const CMeshEffect_RollCut& rhs)
    : CMeshEffect(rhs)
{

}

HRESULT CMeshEffect_RollCut::Initialize_Prototype()
{

    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CMeshEffect_RollCut::Initialize_Copytype(void* pArg)
{
    CMeshEffect_RollCut::Effect_RollCutDesc* pDesc = static_cast<CMeshEffect_RollCut::Effect_RollCutDesc*>(pArg);
    fRotationPerSpeed = pDesc->fRoationPerSec;




    

    if (FAILED(__super::Initialize_Copytype(pArg)))
        return E_FAIL;

    return S_OK;
}

void CMeshEffect_RollCut::Update_Priority(_float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);


}

void CMeshEffect_RollCut::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CMeshEffect_RollCut::Update_Late(_float fTimeDelta)
{
    if (!m_bStop)
    {
        m_LocalData.InitRotation.y += fTimeDelta * fRotationPerSpeed;
        if (m_LocalData.InitRotation.y > 360)
            m_LocalData.InitRotation.y = 0.f;

    }

    __super::Update_Late(fTimeDelta);
   

}

void CMeshEffect_RollCut::Update_Render(_float fTimeDelta)
{
    __super::Update_Render(fTimeDelta);

}

HRESULT CMeshEffect_RollCut::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}

CMeshEffect_RollCut* CMeshEffect_RollCut::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
{
    CMeshEffect_RollCut* pInstance = new CMeshEffect_RollCut(_pDevice, _pDeviceContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CMeshEffect_RollCut");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CMeshEffect_RollCut::Clone(void* pArg)
{
    CMeshEffect_RollCut* pInstance = new CMeshEffect_RollCut(*this);

    if (FAILED(pInstance->Initialize_Copytype(pArg)))
    {
        MSG_BOX("Failed to Created : CMeshEffect_RollCut");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CMeshEffect_RollCut::Free()
{
    __super::Free();
}

HRESULT CMeshEffect_RollCut::Bind_ShaderResources()
{
    __super::Bind_ShaderResources();


    return S_OK;
}

void CMeshEffect_RollCut::Play()
{
    __super::Play();
    m_LocalData.InitRotation.y = 0.f;

}

void CMeshEffect_RollCut::Stop()
{
    __super::Stop();
    m_LocalData.InitRotation.y = 0.f;

}

void CMeshEffect_RollCut::Render_DebugImgui()
{
    __super::Render_DebugImgui();

    if (ImGui::DragFloat("RotationPerSec", (float*)&fRotationPerSpeed))
    {
        

    }
}

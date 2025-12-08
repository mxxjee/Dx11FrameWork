#include "CSnow.h"
#include "CGameInstance.h"                                    
#include "CVIBuffer_Particle_Rect.h"
#include "CTexture.h"
#include "CShader.h"
#include "Client_Defines.h"


USING(Client)

CSnow::CSnow(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CGameObject(pDevice,pContext)
{
}

CSnow::CSnow(const CSnow& Prototype)
    : CGameObject(Prototype)
{
}

HRESULT CSnow::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CSnow::Initialize_Copytype(void* pArg)
{
    if(FAILED(__super::Initialize_Copytype(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;



    return S_OK;
}

void CSnow::Update_Priority(_float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
}

void CSnow::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
    m_pVIBuffer->Drop(fTimeDelta);
}

void CSnow::Update_Late(_float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);
}

void CSnow::Update_Render(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderObject(ENUM_TO_UINT(RENDERGROUP::NONALPHA), this);

}

HRESULT CSnow::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    if (FAILED(m_pShader->Begin("Default")))
        return E_FAIL;

    if (FAILED(m_pVIBuffer->Bind_Resource()))
        return E_FAIL;          //IA´Ü°è

    if (FAILED(m_pVIBuffer->Render()))
        return E_FAIL;

    return S_OK;

}

HRESULT CSnow::Ready_Components()
{
    /* For.Com_VIBuffer */
    CComponent::COMPONENT_DESC Desc;
    Desc.pOwner = this;
  
    CComponent* pVIBuffer_Particle = dynamic_cast<CVIBuffer_Particle_Rect*>(m_pGameInstance->Clone_Prototype(
        PROTOTYPE::COMPONENT, 0, PROTO_COMPONENT_NAME(L"VIBuffer_Particle_Rect"),&Desc)
        );

    if (FAILED(Add_Component(
        COMPONENT_TYPE::PARTICLE,
        pVIBuffer_Particle,
        reinterpret_cast<CComponent**>(&m_pVIBuffer)
    )))
        return E_FAIL;
    

    /* For.Com_Shader */
    m_pShader = m_pGameInstance->Find_Shader(L"VtxPosTexParticle");
    Safe_AddRef(m_pShader);


    /* For.Com_Texture */
    m_pTexture = m_pGameInstance->Find_Texture(L"Snow");
    Safe_AddRef(m_pTexture);



    return S_OK;
}

HRESULT CSnow::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShader, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pTexture->Bind_ShaderResource(m_pShader, "texture0", 0)))
        return E_FAIL;

    return S_OK;
}

CSnow* CSnow::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{

    CSnow* pInstance = new CSnow(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CSnow");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CSnow::Clone(void* pArg)
{
    CSnow* pInstance = new CSnow(*this);

    if (FAILED(pInstance->Initialize_Copytype(pArg)))
    {
        MSG_BOX("Failed to Created : CSnow");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CSnow::Free()
{
    __super::Free();

    Safe_Release(m_pTexture);
    Safe_Release(m_pShader);
    Safe_Release(m_pVIBuffer);
}

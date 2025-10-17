#include "CTerrain.h"
#include "CGameInstance.h"

#include "CVIBuffer_Terrain.h"
#include "CTexture.h"
#include "CShader.h"

#include "Client_Defines.h"


USING(Client)
CTerrain::CTerrain(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CGameObject{pDevice,pContext}
{
}

CTerrain::CTerrain(const CTerrain& Prototype)
    :CGameObject(Prototype)
{
}

HRESULT CTerrain::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;


    return S_OK;
}

HRESULT CTerrain::Initialize_Copytype(void* pArg)
{
    if (FAILED(__super::Initialize_Copytype(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Resources(pArg)))
        return E_FAIL;


    return S_OK;
}

void CTerrain::Update_Priority(_float fTimeDelta)
{
}

void CTerrain::Update(_float fTimeDelta)
{
}

void CTerrain::Update_Late(_float fTimeDelta)
{
}

void CTerrain::Update_Render(_float fTimeDelta)
{
    __super::Update_Render(fTimeDelta); 
    m_pGameInstance->Add_RenderObject(ENUM_TO_UINT(RENDERGROUP::PRIORITY), this);

}

HRESULT CTerrain::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    if(FAILED(m_pShader->Begin(m_passName)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Resource()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CTerrain::Ready_Components(void* pArg)
{

    CComponent* pBuffer_Terrain = dynamic_cast<CVIBuffer_Terrain*>(m_pGameInstance->Clone_Prototype
    (PROTOTYPE::COMPONENT, 0, PROTO_COMPONENT_NAME(L"VIBuffer_Terrain"), pArg));


    if(FAILED(__super::Add_Component(COMPONENT_TYPE::VIBUFFER_TERRAIN,pBuffer_Terrain,(CComponent**)&m_pVIBufferCom)))
        return E_FAIL;

    return S_OK;
}

HRESULT CTerrain::Ready_Resources(void* pArg)
{
    CheckNullResult(pArg, E_FAIL);
    TERRAIN_DESC* pTerrain_Desc = static_cast<TERRAIN_DESC*>(pArg);
    m_ShaderName = pTerrain_Desc->ShaderName;
    m_passName = pTerrain_Desc->passName;


    m_pTexture = CTexture::Create(m_pDevice, m_pContext, pTerrain_Desc->ImgPath.c_str(), 1);


    m_pShader = m_pGameInstance->Find_Shader(pTerrain_Desc->ShaderName);
    Safe_AddRef(m_pShader);

  
    return S_OK;
}

HRESULT CTerrain::Bind_ShaderResources()
{
    if(FAILED(m_pTransformCom->Bind_ShaderResource(m_pShader,"g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pTexture->Bind_ShaderResource(m_pShader, "texture0",0)))
        return E_FAIL;


       
    return S_OK;
}

CTerrain* CTerrain::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
    CTerrain* pInstance = new CTerrain(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create :CTerrain ");
        Safe_Release(pInstance);

    }



    return pInstance;
}

CGameObject* CTerrain::Clone(void* pArg)
{
    CTerrain* pInstance = new CTerrain(*this);
    if (FAILED(pInstance->Initialize_Copytype(pArg)))
    {
        MSG_BOX("Failed to Cloned :CTerrain ");
        Safe_Release(pInstance);

    }
    return pInstance;
}

void CTerrain::Free()
{
    __super::Free();

    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pTexture);
    Safe_Release(m_pShader);
}

#include "CTerrain_Base.h"
#include "CGameInstance.h"

#include "CShader.h"
#include "CTexture.h"
#include "CVIBuffer.h"

#include "CCamera_Base.h"


CTerrain_Base::CTerrain_Base(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CGameObject{ pDevice,pContext }
{
}

CTerrain_Base::CTerrain_Base(const CTerrain_Base& Prototype)
    : CGameObject(Prototype)
{
}

HRESULT CTerrain_Base::Initialize_Prototype()
{

    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;


    return S_OK;
}

HRESULT CTerrain_Base::Initialize_Copytype(void* pArg)
{
    if (FAILED(__super::Initialize_Copytype(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Resources(pArg)))
        return E_FAIL;


    return S_OK;
}

void CTerrain_Base::Update_Priority(_float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);

}

void CTerrain_Base::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CTerrain_Base::Update_Late(_float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);
}

void CTerrain_Base::Update_Render(_float fTimeDelta)
{
    __super::Update_Render(fTimeDelta);
    if(Is_Visible())
        m_pGameInstance->Add_RenderObject(m_eRenderGroup, this);

}

HRESULT CTerrain_Base::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    if (FAILED(m_pShader->Begin(m_passName)))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Bind_Resource()))
        return E_FAIL;

    if (FAILED(m_pVIBufferCom->Render()))
        return E_FAIL;

    return S_OK;
}


HRESULT CTerrain_Base::Ready_Resources(void* pArg)
{
    CheckNullResult(pArg, E_FAIL);
    TERRAIN_DESC* pTerrain_Desc = static_cast<TERRAIN_DESC*>(pArg);
    m_ShaderName = pTerrain_Desc->ShaderName;
    m_passName = pTerrain_Desc->passName;
    m_eRenderGroup = pTerrain_Desc->eRenderGroup;


    m_pTexture = m_pGameInstance->Find_Texture(pTerrain_Desc->TextureKey);
    if (m_pTexture)
        Safe_AddRef(m_pTexture);


    m_pShader = m_pGameInstance->Find_Shader(pTerrain_Desc->ShaderName);
    Safe_AddRef(m_pShader);


    return S_OK;
}

HRESULT CTerrain_Base::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShader, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pTexture->Bind_ShaderResource(m_pShader, "g_DiffuseTexture", 0)))
        return E_FAIL;



    return S_OK;
}



bool CTerrain_Base::Is_Visible()
{
    CCamera_Base* pMaincamera = m_pGameInstance->Get_MainCamera();
    CheckNullResult(pMaincamera, false);

    if (pMaincamera->IsInDistance(m_TerrainChunk.vCenter))
    {
        if (pMaincamera->IsInFrustum(m_TerrainChunk.ChunkBound.MinBound,
                                    m_TerrainChunk.ChunkBound.MaxBound))
            return true;
    }

    return false;
}

void CTerrain_Base::Free()
{
    __super::Free();
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pTexture);
    Safe_Release(m_pShader);

}

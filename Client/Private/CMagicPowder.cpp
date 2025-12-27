#include "CMagicPowder.h"
#include "CGameInstance.h"                                    
#include "CVIBuffer_Particle_Point.h"
#include "CTexture.h"
#include "CShader.h"
#include "Client_Defines.h"
#include "CCollider_Base.h"
#include "CBounding_AABB.h"
#include "CLayer.h"

USING(Client)

CMagicPowder::CMagicPowder(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CGameObject(pDevice, pContext)
{
}

CMagicPowder::CMagicPowder(const CMagicPowder& Prototype)
    : CGameObject(Prototype)
{
}

HRESULT CMagicPowder::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CMagicPowder::Initialize_Copytype(void* pArg)
{
    MAGICPOWDER_DESC* pDesc = static_cast<MAGICPOWDER_DESC*>(pArg);

    if (FAILED(__super::Initialize_Copytype(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_fLifeTime = pDesc->fLifeTime;
    m_pGameInstance->Invoke(m_fLifeTime, false, false, false,[this]()
        {
            CheckFalse(m_bActive);
            Set_Active(false);
            CGameInstance::GetInstance()->UnRegister_Collider(m_pCollider,m_iSceneID);

            
            CLayer* pLayer = m_pGameInstance->Find_Layer(m_iSceneID, L"Particle_Layer");
            CheckNull(pLayer);
            pLayer->RequestDestroy(this);
        },this);


    return S_OK;
}

void CMagicPowder::Update_Priority(_float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
}

void CMagicPowder::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
    m_pVIBuffer->Spread(fTimeDelta);
}

void CMagicPowder::Update_Late(_float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);
    if (m_pCollider)
        m_pCollider->Update_Collider(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));


}

void CMagicPowder::Update_Render(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderObject(ENUM_TO_UINT(RENDERGROUP::NONALPHA), this);

#ifdef _DEBUG
    if (CGameInstance::m_bDrawDebug)
    {
        if (FAILED(m_pGameInstance->Add_DebugComponent(m_pCollider)))
            return;
    }

#endif // _DEBUG

}

HRESULT CMagicPowder::Render()
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

HRESULT CMagicPowder::Ready_Components()
{/* For.Com_VIBuffer */
    CComponent::COMPONENT_DESC Desc;
    Desc.pOwner = this;

    CComponent* pVIBuffer_Particle = dynamic_cast<CVIBuffer_Particle_Point*>(m_pGameInstance->Clone_Prototype(
        PROTOTYPE::COMPONENT, 0, PROTO_COMPONENT_NAME(L"VIBuffer_Particle_Point"), &Desc)
        );

    if (FAILED(Add_Component(
        COMPONENT_TYPE::PARTICLE,
        pVIBuffer_Particle,
        reinterpret_cast<CComponent**>(&m_pVIBuffer)
    )))
        return E_FAIL;


    /* For.Com_Shader */
    m_pShader = m_pGameInstance->Find_Shader(L"VtxPosParticle");
    Safe_AddRef(m_pShader);


    /* For.Com_Texture */
    m_pTexture = m_pGameInstance->Find_Texture(L"Snow");
    Safe_AddRef(m_pTexture);


    CCollider_Base::COLLIDER_DESC ColDesc;
    ColDesc.m_eColGroup = ENUM_TO_UINT(COLLISION_GROUP::PARTICLE);
    ColDesc.pOwner = this;

    CBounding_AABB::BOUNDING_AABB_DESC aabbDesc;
    aabbDesc.vCenter = _float3(0.5f, 0.5f, 0.5f);
    aabbDesc.Extents = _float3(1.f,1.f,1.f);
    ColDesc.m_BoundingDesc = &aabbDesc;
    ColDesc.m_iLevelID = m_iSceneID;

    m_pCollider = dynamic_cast<CCollider_Base*>(m_pGameInstance->Clone_Prototype(
        PROTOTYPE::COMPONENT,
        0,
        PROTO_COMPONENT_NAME(L"BoxCollider"),
        &ColDesc)
        );

    m_pCollider->Set_Trigger(true);

    return S_OK;
}

HRESULT CMagicPowder::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShader, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShader->Bind_Vector("g_CamPosition", CGameInstance::GetInstance()->Get_CamPosition(ENUM_TO_UINT(CAMERA_TYPE::TARGET)))))
        return E_FAIL;

    if (FAILED(m_pTexture->Bind_ShaderResource(m_pShader, "texture0", 0)))
        return E_FAIL;

    return S_OK;
}

CMagicPowder* CMagicPowder::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
    CMagicPowder* pInstance = new CMagicPowder(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CMagicPowder");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CMagicPowder::Clone(void* pArg)
{
    CMagicPowder* pInstance = new CMagicPowder(*this);

    if (FAILED(pInstance->Initialize_Copytype(pArg)))
    {
        MSG_BOX("Failed to Created : CMagicPowder");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CMagicPowder::Free()
{
    __super::Free();
    
    Safe_Release(m_pCollider);
    Safe_Release(m_pTexture);
    Safe_Release(m_pShader);
    Safe_Release(m_pVIBuffer);

}


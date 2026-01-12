#include "CMagicPowder.h"
#include "CGameInstance.h"                                    
#include "CVIBuffer_Particle_Point.h"
#include "CTexture.h"
#include "CShader.h"
#include "Client_Defines.h"
#include "CCollider_Base.h"
#include "CBounding_AABB.h"
#include "CLayer.h"
#include "CParticle.h"
#include "CEffectPoolManager.h"
#include "CGameManager.h"
#include "CBody.h"
#include "CPlayer.h"
#include "CModel.h"



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

    CParticle::PARTICLE_DESC MagicPowderDesc;
    MagicPowderDesc.ProtoName = L"Particle";
    MagicPowderDesc.DataName = L"MagicPowder";
    MagicPowderDesc.eRenderGroup = ENUM_TO_UINT(RENDERGROUP::NONLIGHT);
    MagicPowderDesc.passName = "Smoke";
    MagicPowderDesc.ShaderName = L"VtxPosParticle";
    MagicPowderDesc.ObjTag = L"MagicPowder";

    m_pEffect = CEffectPoolManager::GetInstance()->Request_Spawn(L"Particle", &MagicPowderDesc);
    if (m_pEffect)
    {
        CPlayer* pPlayer = CGameManager::GetInstance()->Get_MainPlayer();
        if (pPlayer)
        {
   
            m_pEffect->Set_OrigniMatrix(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));
            m_pEffect->Play();
        }
      
    }


    /////////////////////¹ÝÂ¦ÀÌ

    CParticle::PARTICLE_DESC TwinkleDesc;
    TwinkleDesc.ProtoName = L"Particle";
    TwinkleDesc.DataName = L"Twinkle";
    TwinkleDesc.eRenderGroup = ENUM_TO_UINT(RENDERGROUP::NONLIGHT);
    TwinkleDesc.passName = "Smoke";
    TwinkleDesc.ShaderName = L"VtxPosParticle";
    TwinkleDesc.ObjTag = L"Twinkle";

    m_pEffect = CEffectPoolManager::GetInstance()->Request_Spawn(L"Particle", &TwinkleDesc);
    if (m_pEffect)
    {
        CPlayer* pPlayer = CGameManager::GetInstance()->Get_MainPlayer();
        if (pPlayer)
        {

            m_pEffect->Set_OrigniMatrix(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr()));
            m_pEffect->Play();
        }

    }
    m_fLifeTime = pDesc->fLifeTime;
    m_pGameInstance->Invoke(m_fLifeTime, false, false, false,[this]()
        {
            CheckFalse(m_bActive);
            Set_Active(false);
            CGameInstance::GetInstance()->UnRegister_Collider(m_pCollider,m_iSceneID);

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


}

HRESULT CMagicPowder::Render()
{
#ifdef _DEBUG
    if (CGameInstance::m_bDrawDebug)
    {
        if (m_pCollider)
            m_pCollider->Render();

    }
#endif // _DEBUG

    return S_OK;
}

HRESULT CMagicPowder::Ready_Components()
{/* For.Com_VIBuffer */
    CComponent::COMPONENT_DESC Desc;
    Desc.pOwner = this;

  

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


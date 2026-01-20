#include "CCollider_Base.h"
#include "CGameInstance.h"
#include "CBounding.h"
#include "CCamera_Base.h"



CCollider_Base::CCollider_Base(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CComponent(pDevice,pContext)
{


}

CCollider_Base::CCollider_Base(const CCollider_Base& Prototype)
    : CComponent(Prototype),
    m_WolrdMatrix(Prototype.m_WolrdMatrix),
    m_bActive(Prototype.m_bActive),
    m_bDebugDraw(Prototype.m_bDebugDraw),
    m_eType(Prototype.m_eType)
#ifdef  _DEBUG
    ,m_pBatch(Prototype.m_pBatch),
    m_pEffect(Prototype.m_pEffect),
    m_pInputLayout(Prototype.m_pInputLayout)

#endif //  _DEBUG
{
    m_WolrdMatrix = XMMatrixIdentity();
}


HRESULT CCollider_Base::Initialize_Prototype()
{
#ifdef _DEBUG
    m_pBatch = new PrimitiveBatch<VertexPositionColor>(m_pContext.Get());
    m_pEffect = new BasicEffect(m_pDevice.Get());
    m_pEffect->SetVertexColorEnabled(true);

    const void* pShaderInput = {};
    size_t iShaderInputLength = {};

    m_pEffect->GetVertexShaderBytecode(&pShaderInput, &iShaderInputLength);

    if (FAILED(m_pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount,
        pShaderInput, iShaderInputLength, &m_pInputLayout)))
        return E_FAIL;


#endif // DEBUG

    return S_OK;
}

HRESULT CCollider_Base::Initialize_Copytype(void* pArg)
{
    if(FAILED(__super::Initialize_Copytype(pArg)))
        return E_FAIL;

    COLLIDER_DESC* pDesc = static_cast<COLLIDER_DESC*>(pArg);
    if (pDesc)
    {
        m_eColGroup = pDesc->m_eColGroup;
        m_iLevelID = pDesc->m_iLevelID;
    }
    if(m_pGameInstance->Get_EngineDesc().eEngineMode==EngineMode::CLIENT)
        m_pGameInstance->Register_Collider(this, m_iLevelID);



    return S_OK;
}

HRESULT CCollider_Base::Update_Collider(XMMATRIX    WorldMatrix)
{
    return S_OK;
}

bool CCollider_Base::Intersects_Ray(_vector origin, _vector rayDir, _float& Dist, CTransform* pTransform)
{
    return false;
}


bool CCollider_Base::Intersect(CCollider_Base* pOther)
{
    m_isColl = m_pBounding->Intersect(pOther->m_eType, pOther->m_pBounding);

    return m_isColl;
}
void CCollider_Base::OnCollisionEnter(_uint iGroup, CCollider_Base* pOther)
{                    
  

    if (m_pOwner)
        m_pOwner->OnCollisionEnter(iGroup,pOther);
 
}
void CCollider_Base::OnCollision(_uint iGroup, CCollider_Base* pOther)
{
   

    if (m_pOwner)
        m_pOwner->OnCollisionStay(iGroup,pOther);
}

void CCollider_Base::OnCollisionExit(_uint iGroup, CCollider_Base* pOther)
{
    if (m_pOwner)
        m_pOwner->OnCollisionExit(iGroup,pOther);
}

_float3 CCollider_Base::Get_MaxBound(_vector vCenter)
{
    return m_pBounding->Get_MaxBound(vCenter);
}

_float3 CCollider_Base::Get_MinBound(_vector vCenter)
{
    return m_pBounding->Get_MinBound(vCenter);
}




#ifdef _DEBUG
HRESULT CCollider_Base::Render()
{
    CheckFalseResult(m_bActive,E_FAIL);

    CCamera_Base* pMainCamera = m_pGameInstance->Get_MainCamera();
    CheckNullResult(pMainCamera,E_FAIL);

    m_pContext->GSSetShader(nullptr, nullptr, 0);

    m_pEffect->SetWorld(XMMatrixIdentity());
    m_pEffect->SetView(XMLoadFloat4x4(&m_pGameInstance->Get_ViewMatrix(ENUM_TO_UINT(pMainCamera->Get_CameraType()))));
    m_pEffect->SetProjection(XMLoadFloat4x4(&m_pGameInstance->Get_ProjMatrix(ENUM_TO_UINT(pMainCamera->Get_CameraType()))));

    m_pEffect->Apply(m_pContext.Get());
    m_pContext->IASetInputLayout(m_pInputLayout);

    m_pBatch->Begin();

    m_pBounding->Render(m_pBatch, m_isColl);

    m_pBatch->End();
    return S_OK;
}
#endif


void CCollider_Base::Free()
{

   
    __super::Free();
  #ifdef _DEBUG
    if (false == m_isClone)
    {
        Safe_Delete(m_pBatch);
        Safe_Delete(m_pEffect);
        Safe_Release(m_pInputLayout);
    }

#endif // DEBUG

    
    Safe_Release(m_pBounding);
   
}

void CCollider_Base::RegisterCurrentCollision(CCollider_Base* pOther)
{
    CheckNull(pOther);
    m_CurrCollisions.insert(pOther);

}

void CCollider_Base::ResolveEvents()
{
    for (auto* pOther : m_CurrCollisions)
    {
        if (m_PrevCollisions.find(pOther) == m_PrevCollisions.end())
            OnCollisionEnter(pOther->Get_ColGroup(),pOther);
        else
            OnCollision(pOther->Get_ColGroup(), pOther);

    }

    // Exit
    for (auto* pOther : m_PrevCollisions)
    {
        if (m_CurrCollisions.find(pOther) == m_CurrCollisions.end())
            OnCollisionExit(pOther->Get_ColGroup(),pOther);
    }

    // swap
    m_PrevCollisions = m_CurrCollisions;
}

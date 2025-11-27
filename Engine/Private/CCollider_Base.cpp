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
    m_eType(Prototype.m_eType),
#ifdef  _DEBUG
    m_pBatch(Prototype.m_pBatch),
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
   
    return S_OK;
}

HRESULT CCollider_Base::Update_Collider(CTransform* pTransform)
{
    return E_NOTIMPL;
}

bool CCollider_Base::Intersects_Ray(_vector origin, _vector rayDir, _float& Dist, CTransform* pTransform)
{
    return false;
}

#ifdef _DEBUG
HRESULT CCollider_Base::Render()
{
    m_pEffect->SetWorld(XMMatrixIdentity());
    m_pEffect->SetView(XMLoadFloat4x4(&m_pGameInstance->Get_ViewMatrix(ENUM_TO_UINT(m_pGameInstance->Get_RenderCamera()->Get_CameraType()))));
    m_pEffect->SetProjection(XMLoadFloat4x4(&m_pGameInstance->Get_ProjMatrix(ENUM_TO_UINT(m_pGameInstance->Get_RenderCamera()->Get_CameraType()))));

    m_pEffect->Apply(m_pContext.Get());
    m_pContext->IASetInputLayout(m_pInputLayout);

    m_pBatch->Begin();

    m_pBounding->Render(m_pBatch);

    m_pBatch->End();
    return S_OK;
}
#endif


void CCollider_Base::Free()
{
    __super::Free();
    if (false == m_isClone)
    {
        Safe_Delete(m_pBatch);
        Safe_Delete(m_pEffect);
    }

    Safe_Release(m_pBounding);
}

#include "CFontComponent.h"
#include "CFont_Manager.h"
#include "CGameInstance.h"
#include "MathUtils.h"

CFontComponent::CFontComponent(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CComponent(pDevice,pContext)
{
}

CFontComponent::CFontComponent(const CFontComponent& Prototype)
    :CComponent(Prototype)
{
}

HRESULT CFontComponent::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;


    return S_OK;
}

HRESULT CFontComponent::Initialize_Copytype(void* pArg)
{
    if (FAILED(__super::Initialize_Copytype(pArg)))
        return E_FAIL;

    if (FAILED(Create_BS_AlphaBlend()))
        return E_FAIL;


    Font_Desc* pFontDesc = static_cast<Font_Desc*>(pArg);

    m_pFont = m_pGameInstance->Find_Font(pFontDesc->FontName);
    CheckNullResult(m_pFont, E_FAIL);

    m_pBatch = m_pGameInstance->Get_Batch();
    m_vColor = pFontDesc->vDefaultFontColor;
    m_vScale = pFontDesc->fScale;
    m_fRotation = pFontDesc->fRotation;

    m_pText = L"";
    return S_OK;
}

void CFontComponent::Set_Text(wstring _Text)
{
    m_pText = _Text;
}

HRESULT CFontComponent::Render()
{
    m_pContext->GSSetShader(nullptr, nullptr, 0);

    ID3D11BlendState* pPrevBS = nullptr;
    float fPrevBlendFactor[4] = {};
    UINT uPrevSampleMask = 0;

    m_pContext->OMGetBlendState(&pPrevBS, fPrevBlendFactor, &uPrevSampleMask);


    m_pContext->OMSetBlendState(BS_AlphaBlend.Get(), nullptr, 0xFFFFFFFF);

    m_pBatch->Begin();

    m_pFont->DrawString(m_pBatch, 
        m_pText.c_str(), 
        m_vPosition,
        XMLoadFloat4(&m_vColor),
        m_fRotation,
        _float2(0.f,0.f),
        m_vScale);

    m_pBatch->End();

    m_pContext->OMSetBlendState(pPrevBS, fPrevBlendFactor, uPrevSampleMask);
    

    return S_OK;
}

void CFontComponent::Update(XMMATRIX WorldMatrix, _float alpha)
{
    
    _float4x4 fWorldMat;
    XMStoreFloat4x4(&fWorldMat, WorldMatrix);

    tagEngine_Desc EngineDesc = m_pGameInstance->Get_EngineDesc();

  
    
    _vector  vPos=MathUtils::WorldToScreen(XMVectorSet(fWorldMat._41, fWorldMat._42, fWorldMat._43, 1.f),
        m_pGameInstance->Get_ViewMatrix(ENUM_TO_UINT(CAMERA_TYPE::TARGET)),
        m_pGameInstance->Get_ViewMatrix(ENUM_TO_UINT(CAMERA_TYPE::TARGET)),
        EngineDesc.iWinSizeX, EngineDesc.iWinSizeY);

    m_vPosition = _float2(XMVectorGetX(vPos), XMVectorGetY(vPos));
    m_vColor.w = alpha;
}

CFontComponent* CFontComponent::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
    CFontComponent* pInstance = new CFontComponent(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create : CFontComponent");
        Safe_Release(pInstance);
    }


    return pInstance;
}

CComponent* CFontComponent::Clone(void* pArg)
{
    CFontComponent* pInstance = new CFontComponent(*this);
    if (FAILED(pInstance->Initialize_Copytype(pArg)))
    {
        MSG_BOX("Failed to Cloned : CFontComponent");
        Safe_Release(pInstance);
    }


    return pInstance;
}

void CFontComponent::Free()
{
    __super::Free();

    
   

}

HRESULT CFontComponent::Create_BS_AlphaBlend()
{
    D3D11_BLEND_DESC desc;
    memset(&desc, 0, sizeof(desc));
    desc.AlphaToCoverageEnable = false;
    desc.IndependentBlendEnable = false;

    desc.RenderTarget[0].BlendEnable = TRUE;
    desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    
    if(FAILED(m_pDevice->CreateBlendState(&desc, BS_AlphaBlend.GetAddressOf())))
        return E_FAIL;

    

    return S_OK;


}

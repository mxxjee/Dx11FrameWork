#include "CRenderState_Manager.h"
#include "CShader.h"

CRenderState_Manager::CRenderState_Manager(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pContext)
    :m_pDevice(_pDevice), m_pContext(_pContext)
{
}

HRESULT CRenderState_Manager::Initialize()
{
    //Default값 채워넣기
    D3D11_BLEND_DESC Blenddesc = CD3D11_BLEND_DESC(D3D11_DEFAULT);

    CD3D11_SAMPLER_DESC Samplerdesc(D3D11_DEFAULT);
    CD3D11_RASTERIZER_DESC Rasterdesc (D3D11_DEFAULT);

    D3D11_DEPTH_STENCIL_DESC DepthStencildesc = CD3D11_DEPTH_STENCIL_DESC(D3D11_DEFAULT);

    m_pDevice->CreateSamplerState(&Samplerdesc, m_DefaultRenerState._samplerState.GetAddressOf());
    m_pDevice->CreateBlendState(&Blenddesc, m_DefaultRenerState._BlendState.GetAddressOf());
    m_pDevice->CreateRasterizerState(&Rasterdesc, m_DefaultRenerState._rasterizerState.GetAddressOf());
    m_pDevice->CreateDepthStencilState(&DepthStencildesc, m_DefaultRenerState._DepthStencilState.GetAddressOf());

 

    return S_OK;
}

HRESULT CRenderState_Manager::Register_RenderStates(_uint iRenderGroup, const RenderStates& States)
{
    auto iter = m_RenderStates.find(iRenderGroup);
    if (iter != m_RenderStates.end())
        return E_FAIL;

    m_RenderStates.emplace(iRenderGroup, States);
    return S_OK;
}

const RenderStates& CRenderState_Manager::Get_RenderStates(_uint iRenderGroup)
{
    auto iter = m_RenderStates.find(iRenderGroup);
    if (iter != m_RenderStates.end())
        return iter->second;

    return m_DefaultRenerState;


}

HRESULT CRenderState_Manager::Bind_SamplerState(CShader* pShader, _uint iRenderGroup)
{
    if (FAILED(pShader->Bind_Sampler("sampler0", m_RenderStates[iRenderGroup]._samplerState, 0)))
        return E_FAIL;

    return S_OK;
}

CRenderState_Manager* CRenderState_Manager::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pContext)
{
    CRenderState_Manager* pInstance = new CRenderState_Manager(_pDevice, _pContext);
    if(FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Create : CRenderState_Manager");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CRenderState_Manager::Free()
{
   
    m_RenderStates.clear();
}

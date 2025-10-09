#include "CQuad.h"
#include "CGameInstance.h"
#include "CTexture.h"
#include "CShader.h"
#include "CVIBuffer_Rect.h"


CQuad::CQuad(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CGameObject(pDevice,pContext)
{
}

CQuad::CQuad(const CQuad& rhs)
	:CGameObject(rhs)
{
}

HRESULT CQuad::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CQuad::Initialize_Copytype(void* pArg)
{
    /*부모 컴포넌트 값세팅 */
    if (FAILED(__super::Initialize_Copytype(pArg)))
        return E_FAIL;

    //RectBuffer생성
    CBase* pBuffer = m_pGameInstance->Clone_Prototype(PROTOTYPE::COMPONENT, 0, PROTO_COMPONENT_NAME(L"VIBuffer_Rect"), pArg);
    if (pBuffer)
    {
        m_pVIBufferCom = dynamic_cast<CVIBuffer_Rect*>(pBuffer);
        
        if (m_pVIBufferCom)
        {
            m_pVIBufferCom->AddRef();
            m_Components.emplace(L"VIBuffer_Rect", m_pVIBufferCom);

        }

    }

    m_pTexShader = m_pGameInstance->Find_Shader(L"Default");

    QUAD_DESC* pQuad_Desc = static_cast<QUAD_DESC*>(pArg);
    m_eRenderGroup = pQuad_Desc->eRenderGroup;

    CreateRasterizerState();
    CreateSamplerState();
    CreateBlendState();

    m_pTexture = new CTexture(m_pDevice);
    if(FAILED(m_pTexture->LoadImg(pQuad_Desc->ImgPath)))
        return E_FAIL;



    return S_OK;
}

void CQuad::Update_Priority(_float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
}

void CQuad::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
    CheckNull(m_pTransformCom);

   

}

void CQuad::Update_Late(_float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);
}

void CQuad::Update_Render(_float fTimeDelta)
{
    __super::Update_Render(fTimeDelta);
    m_pGameInstance->Add_RenderObject(m_eRenderGroup, this);

}

HRESULT CQuad::Render()
{
    __super::Render();

  //렌더 할때 copydata로 GPU에게 데이터전송
    m_pTexShader->SetMatrix("g_WorldMatrix", m_pTransformCom->Get_World((TransformScope::WORLD)));

    _float4x4 _fViewProj;
    XMStoreFloat4x4(&_fViewProj, m_pGameInstance->GetMulViewProjMatrix(m_eRenderGroup == RENDERGROUP::UI));


    m_pTexShader->SetMatrix("g_ViewProjMatrix", _fViewProj);
    m_pTexShader->SetResource("texture0", m_pTexture->GetComPtr());
    m_pTexShader->SetSampler("sampler0", m_RenderStates._samplerState);


    //IA단계
    m_pVIBufferCom->Bind_Resource();
    
    //VS-PS
    m_pTexShader->Apply();

    //RS단계
    Set_RasterizerState();
    
    Set_BlendState();           //OM단계
    m_pVIBufferCom->Render();      //OM단계
    return S_OK;
}






HRESULT CQuad::CreateRasterizerState()
{
    D3D11_RASTERIZER_DESC desc;
    memset(&desc, 0, sizeof(desc));

    desc.FillMode = D3D11_FILL_SOLID;//WIREFRAME of SOLID
    desc.CullMode = D3D11_CULL_NONE;//CULLMODE: 반시계 컬링
    desc.FrontCounterClockwise = false;
    desc.DepthClipEnable = true;

    HRESULT hr = m_pDevice->CreateRasterizerState(&desc, m_RenderStates._rasterizerState.GetAddressOf());
    CHECK(hr);

    return S_OK;
}

HRESULT CQuad::CreateSamplerState()
{
    D3D11_SAMPLER_DESC desc;
    memset(&desc, 0, sizeof(desc));
    desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    desc.BorderColor[0] = 0.0f;
    desc.BorderColor[1] = 0.0f;
    desc.BorderColor[2] = 0.0f;
    desc.BorderColor[3] = 0.0f;


    desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    desc.MaxAnisotropy = 16;
    desc.MaxLOD = FLT_MAX;
    desc.MinLOD = FLT_MIN;
    desc.MipLODBias = 0.0f;

    m_pDevice->CreateSamplerState(&desc, m_RenderStates._samplerState.GetAddressOf());

    return S_OK;
}

HRESULT CQuad::CreateBlendState()
{
    D3D11_BLEND_DESC desc;
    memset(&desc, 0, sizeof(desc));
    desc.AlphaToCoverageEnable = false;
    desc.IndependentBlendEnable = false;

    desc.RenderTarget[0].BlendEnable = true;
    desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    HRESULT hr = m_pDevice->CreateBlendState(&desc, m_RenderStates._BlendState.GetAddressOf());
    CHECK(hr);
    return S_OK;
}


void CQuad::Set_RasterizerState()
{
    m_pContext.Get()->RSSetState(m_RenderStates._rasterizerState.Get());

}


void CQuad::Set_BlendState()
{
    m_pContext.Get()->OMSetBlendState(m_RenderStates._BlendState.Get(), nullptr, 0xFFFFFFFF);
    
}

CQuad* CQuad::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
{
    CQuad* pInstance = new CQuad(_pDevice, _pDeviceContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create :CQuad ");
        Safe_Release(pInstance);

    }



    return pInstance;
}

CGameObject* CQuad::Clone(void* pArg)
{
    CQuad* pInstance = new CQuad(*this);
    if (FAILED(pInstance->Initialize_Copytype(pArg)))
    {
        MSG_BOX("Failed to Cloned :CQuad ");
        Safe_Release(pInstance);

    }
    return pInstance;
}

void CQuad::Free()
{
    __super::Free();
    Safe_Release(m_pTexture);
    Safe_Release(m_pVIBufferCom);
}

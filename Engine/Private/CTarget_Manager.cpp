#include "CTarget_Manager.h"
#include "CRenderTarget.h"
#include "CShader.h"
#include "CGameInstance.h"

CTarget_Manager::CTarget_Manager(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :m_pDevice(pDevice),m_pContext(pContext)
{
}

HRESULT CTarget_Manager::Initialize()
{
    
    m_pBackBuffer = CGameInstance::GetInstance()->Get_BackBuffer_RTV();
    m_pDSV = CGameInstance::GetInstance()->Get_BackBuffer_DSV();

    return S_OK;
}

HRESULT CTarget_Manager::Add_RenderTarget(const _wstring& strTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vClearColor)
{
    CRenderTarget* pRenderTarget = Find_RenderTarget(strTargetTag);
    if (pRenderTarget != nullptr)
        return E_FAIL;

    CRenderTarget* pNewRenderTarget = CRenderTarget::Create(m_pDevice, m_pContext, iWidth, iHeight, ePixelFormat, vClearColor);
    CheckNullResult(pNewRenderTarget, E_FAIL);

    m_RenderTargets.emplace(strTargetTag, pNewRenderTarget);

    return S_OK;
}

HRESULT CTarget_Manager::Add_MRT(const _wstring& strMRTTag, const _wstring& strTargetTag)
{
    //그룹에 추가하는 함수
    CRenderTarget* pRenderTarget = Find_RenderTarget(strTargetTag);
    CheckNullResult(pRenderTarget,E_FAIL);


    //그룹검색 -> 있으며 바로추가, 없으면 생성해서추가
    list<CRenderTarget*>* pMRTList = Find_MRT(strMRTTag);
    if (nullptr == pMRTList)
    {
        list<CRenderTarget*>     MRTList;
        MRTList.push_back(pRenderTarget);
        m_MRTs.emplace(strMRTTag, MRTList);
    }
    else
        pMRTList->push_back(pRenderTarget);

  
    return S_OK;
}

HRESULT CTarget_Manager::Begin_MRT(const _wstring& strMRTTag, ID3D11DepthStencilView* pDSV)
{
    m_pContext->PSSetShader(nullptr, nullptr, 0);
    //내가 지정한 멀티렌더타겟안에있는 타겟들을 순서대로 장치에 동시에 바인딩한다
    list<CRenderTarget*>*     pMRTList = Find_MRT(strMRTTag);
    CheckNullResult(pMRTList, E_FAIL);

    ////현재 ㅏㅂ인딩되어있는 백버퍼를 저장한다.
    //m_pContext->OMGetRenderTargets(1, m_pBackBuffer.ReleaseAndGetAddressOf(), m_pDSV.ReleaseAndGetAddressOf());

    ID3D11RenderTargetView* pRTVs[8] = { nullptr };
    _uint   iNumRenderTargets = { };

    for (auto& RenderTarget : *pMRTList)
    {
        RenderTarget->Clear();
        pRTVs[iNumRenderTargets++] = RenderTarget->Get_RTV().Get();
    }
    
    if (nullptr != pDSV)
    {
        m_pContext->ClearDepthStencilView(pDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
    }

    m_pContext->OMSetRenderTargets(iNumRenderTargets, pRTVs, nullptr == pDSV ? m_pDSV.Get() : pDSV);


    return S_OK;
}

HRESULT CTarget_Manager::End_MRT()
{
    ID3D11ShaderResourceView* nullSRV[16] = {};
    m_pContext->VSSetShaderResources(0, 16, nullSRV);
    m_pContext->PSSetShaderResources(0, 16, nullSRV);
    m_pContext->GSSetShaderResources(0, 16, nullSRV);
    m_pContext->CSSetShaderResources(0, 16, nullSRV);


    ID3D11RenderTargetView* pRTV = m_pBackBuffer.Get();
    m_pContext->OMSetRenderTargets(1, &pRTV, m_pDSV.Get());

    return S_OK;
}

HRESULT CTarget_Manager::Bind_RT_ShaderResource(const _wstring& strTargetTag, CShader* pShader, const _char* pConstantName)
{
    CRenderTarget* pRenderTarget = Find_RenderTarget(strTargetTag);
    CheckNullResult(pRenderTarget, E_FAIL);

  /*  m_pContext->OMSetRenderTargets(0, nullptr, nullptr);
    ID3D11ShaderResourceView* nullSRV[16] = {};

    m_pContext->VSSetShaderResources(0, 16, nullSRV);
    m_pContext->PSSetShaderResources(0, 16, nullSRV);
    m_pContext->GSSetShaderResources(0, 16, nullSRV);
    m_pContext->CSSetShaderResources(0, 16, nullSRV);*/
    return pRenderTarget->Bind_ShaderResource(pShader,pConstantName);
}

#ifdef  _DEBUG
HRESULT CTarget_Manager::Ready_Debug(const _wstring& strTargetTag, _float fX, _float fY, _float fSizeX, _float fSizeY)
{
    CRenderTarget* pRenderTarget = Find_RenderTarget(strTargetTag);
    if (nullptr == pRenderTarget)
        return E_FAIL;

    pRenderTarget->Ready_Debug(fX, fY, fSizeX, fSizeY);

    return S_OK;
}

HRESULT CTarget_Manager::Render(const _wstring& strMRTTag, CShader* pShader, CVIBuffer_Rect* pVIBuffer)
{
    list<CRenderTarget*>* pMRTList = Find_MRT(strMRTTag);
    if (nullptr == pMRTList)
        return E_FAIL;

    for (auto& pRenderTarget : *pMRTList)
    {
        pRenderTarget->Render(pShader, pVIBuffer);
    }

    return S_OK;
}
#endif //  _DEBUG



CRenderTarget* CTarget_Manager::Find_RenderTarget(const _wstring& strTargetTag)
{
    auto    iter = m_RenderTargets.find(strTargetTag);
    if (iter == m_RenderTargets.end())
        return nullptr;

    return iter->second;
}

HRESULT CTarget_Manager::Unbind_RT_ShaderResource(const _wstring& strTargetTag, CShader* pShader, const _char* pConstantName)
{
    CRenderTarget* pRenderTarget = Find_RenderTarget(strTargetTag);
    CheckNullResult(pRenderTarget, E_FAIL);
    ComPtr<ID3D11ShaderResourceView> pNullSRV = nullptr;
    // (2) 해당 쉐이더 변수에 NULL을 바인딩하여 해제합니다.
    return pShader->Bind_SRV(pConstantName, pNullSRV); // 핵심: nullptr을 넘긴다!
}

list<class CRenderTarget*>* CTarget_Manager::Find_MRT(const _wstring& strMRTTag)
{
    auto    iter = m_MRTs.find(strMRTTag);
    if (iter == m_MRTs.end())
        return nullptr;

    return &iter->second;
}

CTarget_Manager* CTarget_Manager::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
    return new CTarget_Manager(pDevice, pContext);
}

void CTarget_Manager::Free()
{
    __super::Free();

    
    for (auto& Pair : m_MRTs)
    {
        for (auto& pRenderTarget : Pair.second)
            Safe_Release(pRenderTarget);
        Pair.second.clear();
    }

    m_MRTs.clear();




 
}

#include "CTarget_Manager.h"
#include "CRenderTarget.h"

CTarget_Manager::CTarget_Manager(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :m_pDevice(pDevice),m_pContext(pContext)
{
}

HRESULT CTarget_Manager::Add_RenderTarget(const _wstring& strTargetTag, _uint iWidth, _uint iHeight, DXGI_FORMAT ePixelFormat, const _float4& vClearColor)
{
    CRenderTarget* pRenderTarget = Find_RenderTarget(strTargetTag);
    if (pRenderTarget != nullptr)
        return E_FAIL;

    CRenderTarget* pNewRenderTarget = CRenderTarget::Create(m_pDevice, m_pContext, iWidth, iHeight, ePixelFormat, vClearColor);
    CheckNullResult(pNewRenderTarget, E_FAIL);

    m_RenderTargets.emplace(strTargetTag, pRenderTarget);

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

    return S_OK;
}

HRESULT CTarget_Manager::Begin_MRT(const _wstring& strMRTTag)
{
    //내가 지정한 멀티렌더타겟안에있는 타겟들을 순서대로 장치에 동시에 바인딩한다
    list<CRenderTarget*>*     pMRTList = Find_MRT(strMRTTag);
    CheckNullResult(pMRTList, E_FAIL);

    //현재 ㅏㅂ인딩되어있는 백버퍼를 저장한다.
    m_pContext->OMGetRenderTargets(0, &m_pBackBuffer, &m_pDSV);

    ID3D11RenderTargetView* pRTVs[8] = { nullptr };
    _uint   iNumRenderTargets = { };

    for (auto& RenderTarget : *pMRTList)
    {
        pRTVs[iNumRenderTargets++] = RenderTarget->Get_RTV().Get();
    }
    
    m_pContext->OMSetRenderTargets(iNumRenderTargets, pRTVs, m_pDSV.Get());

    return S_OK;
}

HRESULT CTarget_Manager::End_MRT()
{
    ID3D11RenderTargetView* pRenderTargets[8] = {
         m_pBackBuffer.Get()
    };

    m_pContext->OMSetRenderTargets(8, pRenderTargets, m_pDSV.Get());

    Safe_Release(m_pBackBuffer);
    Safe_Release(m_pDSV);

    return S_OK;
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

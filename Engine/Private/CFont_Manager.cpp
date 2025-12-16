#include "CFont_Manager.h"


CFont_Manager::CFont_Manager(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :m_pDevice(pDevice),
    m_pContext(pContext)
{
}

HRESULT CFont_Manager::Initialize()
{
    m_pBatch = new SpriteBatch(m_pContext.Get());

    return S_OK;
}

HRESULT CFont_Manager::Add_Font(const _wstring& strFontTag, const _tchar* pFontFilePath)
{

    if (nullptr != Find_Font(strFontTag))
        return E_FAIL;

    SpriteFont* pFont = new::SpriteFont(m_pDevice.Get(), pFontFilePath);
    if (nullptr == pFont)
        return E_FAIL;

    m_Fonts.emplace(strFontTag, pFont);

    return S_OK;
}



SpriteFont* CFont_Manager::Find_Font(const _wstring& strFontTag)
{
    auto    iter = m_Fonts.find(strFontTag);

    if (iter == m_Fonts.end())
        return nullptr;

    return iter->second;
}

CFont_Manager* CFont_Manager::Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
{
    CFont_Manager* pInstance = new CFont_Manager(pDevice, pContext);

    /* 메인앱을 완벽히 사용하기 위한 기타 초기화작업을 수행한다. */
    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CFont_Manager");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CFont_Manager::Free()
{

    __super::Free();

    for (auto& Pair : m_Fonts)
        Safe_Delete(Pair.second);
    m_Fonts.clear();

    Safe_Delete(m_pBatch);


}

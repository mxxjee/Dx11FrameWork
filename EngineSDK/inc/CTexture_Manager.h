#pragma once
#include "CBase.h"

NS_BEGIN(Engine)
class CTexture;

class CTexture_Manager :
    public CBase
{
private:
    CTexture_Manager(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pContext);
    virtual ~CTexture_Manager() = default;
public:
    HRESULT        Initialize();
    HRESULT       Register_Texture(const _wstring & Tag, CTexture* pInstance);

public:
    //폴더안의 텍스처한장씩 로드해서 각가의 texture객체로만든다.(폴더경로, 확장자)
    HRESULT         Load_Textures(const _wstring& FolderPath,const _wstring& Extension);

public:
    static CTexture_Manager* Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pContext);
    virtual void                Free() override;

public:
    CTexture* Find_Texture(const _wstring & Tag);

private:
    UMap<_wstring, CTexture*>    m_mapTex;

private:
    ComPtr<ID3D11Device> m_pDevice = { nullptr };
    ComPtr<ID3D11DeviceContext> m_pContext = { nullptr };
};

NS_END


#pragma once
#include "CBase.h"

NS_BEGIN(Engine)

class CTexture;

class CScreenShot_Manager :
    public CBase
{
private:
    CScreenShot_Manager(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pContext);
    virtual ~CScreenShot_Manager()=default;

    
public:
    void ScreenShot(const _wstring& Key);
    CTexture* Find_ScreenTexture(const _wstring& Key);


public:
    static CScreenShot_Manager* Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pContext);
    virtual void                Free() override;

    HRESULT                 SaveTextureToFile(const _wstring& Key, const _wstring& filePath);

private:
    UMap<_wstring, CTexture*>    m_Texmap;

private:
    ComPtr<ID3D11Device> m_pDevice = { nullptr };
    ComPtr<ID3D11DeviceContext> m_pContext = { nullptr };

};

NS_END

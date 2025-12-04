#pragma once
#include "CBase.h"


NS_BEGIN(Engine)
class CFont :
    public CBase
{
private:
	CFont(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual ~CFont() = default;

public:
	HRESULT Initialize(const _tchar * pFontFilePath);
	HRESULT Draw_Text(SpriteBatch * pBatch, const _tchar * pText, const _float2 & vPosition, _fvector vColor);

private:
	ComPtr<ID3D11Device> m_pDevice = { nullptr };
	ComPtr<ID3D11DeviceContext> m_pContext = { nullptr };
	SpriteFont* m_pFont = { nullptr };

public:
	static CFont* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const _tchar * pFontFilePath);
	virtual void Free() override;

};

NS_END
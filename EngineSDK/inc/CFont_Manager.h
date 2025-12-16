#pragma once
#include "CBase.h"

/*SpriteFont를 관리한다.*/
NS_BEGIN(Engine)
class CFont_Manager final:
    public CBase
{

private:
	CFont_Manager(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual ~CFont_Manager() = default;

public:
	HRESULT Initialize();
	HRESULT Add_Font(const _wstring& strFontTag, const _tchar* pFontFilePath);
public:
	class SpriteFont* Find_Font(const _wstring& strFontTag);
	SpriteBatch* Get_Batch() { return m_pBatch; }
public:
	static CFont_Manager* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);


	virtual void Free() override;
private:
	ComPtr<ID3D11Device> m_pDevice = { nullptr };
	ComPtr<ID3D11DeviceContext> m_pContext = { nullptr };
	SpriteBatch* m_pBatch = { nullptr };
	map<const _wstring, class SpriteFont*>		m_Fonts;

};

NS_END

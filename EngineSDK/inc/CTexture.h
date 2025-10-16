#pragma once
#include "CBase.h"

NS_BEGIN(Engine)
class ENGINE_DLL CTexture:public CBase
{
public:
	CTexture(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pContext);
	CTexture(const CTexture& Prototype);
	~CTexture()=default;
public:
	ComPtr<ID3D11ShaderResourceView> Get_SRV(_uint iIdx);

					//외부에서생성한 SRV를 벡터에넣어줄떄.
	HRESULT			Add_SRV(ComPtr<ID3D11ShaderResourceView> pSRV);

	HRESULT			Add_TextureToSRV(ComPtr<ID3D11Texture2D> pTex);

	HRESULT			Set_SRV(ComPtr<ID3D11ShaderResourceView> pSRV, _uint iIdx);

				//srv를 바로  셰이더로 바인드
	HRESULT			Bind_ShaderResource(class CShader* pShader, const _char* pConstantName, _uint iTextureIndex);

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pTextureFilePath, _uint iNumTextures);
	virtual HRESULT Initialize(void* pArg);

private:
	HRESULT			Load_TextureSequence(const _tchar* pTextureFilePath, _uint iNumTextures);
	HRESULT			Load_Texture(const _tchar* pTextureFilePath);

public:
	//경로로 읽어오는 create (연속적인 텍스처)
	static CTexture* Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pContext, const _tchar* pTextureFilePath, _uint iNumTextures);
	


	static CTexture* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext,ComPtr<ID3D11ShaderResourceView> pSRV);
	static CTexture* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, ComPtr<ID3D11Texture2D> pTex);

	CTexture* Clone(void* pArg);
	virtual void Free() override;
private:
	ComPtr<ID3D11Device>					m_pDevice;
	ComPtr<ID3D11DeviceContext>				m_pContext;

private:
	_uint											m_iNumTextures=1;
	vector<ComPtr<ID3D11ShaderResourceView>>			m_Textures;


};

NS_END
#pragma once
#include "CComponent.h"
#include "CBase.h"

NS_BEGIN(Engine)

class CInputLayout;

class ENGINE_DLL CShader final :
                public CBase
{
public:
	typedef struct tagCShaderInfo
	{
		ComPtr<ID3DX11Effect>			m_pEffect = { nullptr };
		
		ComPtr<ID3DX11EffectTechnique>	m_pTechnique = { nullptr };
		ComPtr<ID3DX11EffectPass>		m_pPass = { nullptr };

		
		ComPtr<ID3DBlob>		m_pErrorBlob = { nullptr };
		ComPtr<ID3DX11EffectMatrixVariable>	m_GlobalViewProj;

		string                m_strTechniqueName = "";
		string                m_strPassName = "";

		tagCShaderInfo() {}
		tagCShaderInfo(const tagCShaderInfo& Prototype)
			:m_pEffect{Prototype.m_pEffect},m_pErrorBlob{Prototype.m_pErrorBlob},m_pTechnique{Prototype.m_pTechnique},
			m_pPass{Prototype.m_pPass}
		{
			m_strTechniqueName = Prototype.m_strTechniqueName;
			m_strPassName = Prototype.m_strPassName;
		}

	}SHADER_INFO;

private:
	CShader(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CShader(const CShader& Prototype);
	virtual ~CShader() = default;

public:
	HRESULT     Initialize_Prototype(const vector<D3D11_INPUT_ELEMENT_DESC>& layout, const wstring& filePath, const string& strTechName, const string& passName);
	HRESULT     Initialize_Copytype(void *pArg);

public:
	static CShader* Create(ComPtr<ID3D11Device> pDevice,
		ComPtr<ID3D11DeviceContext> pContext,
		const vector<D3D11_INPUT_ELEMENT_DESC>& layout,
		const wstring& filePath,
		const string& strTechName,
		const string& passName="");
	virtual CShader* Clone(void* pArg);
	virtual void Free() override;


private:
	/*초기화 및 설정 함수들*/
	HRESULT		LoadShaderFromFile(const wstring & path);
	HRESULT		Set_Technique(const string& strTechName);
	HRESULT		Set_Pass(const string& strPassName);
	HRESULT		Create_InputLayout(const vector<D3D11_INPUT_ELEMENT_DESC>& layout);

public:
	void		SetMatrix(const string& Variable, const _float4x4& mat);
	void		SetVector(const string& Variable, const _float4& vector);
	void		SetFloat(const string& Variable, const _float fValue);
	void		SetResource(const string& Variable, ComPtr<ID3D11ShaderResourceView> resource);
	void		SetSampler(const string& Variable, ComPtr< ID3D11SamplerState> sampler, UINT iIdx = 0);

public:
	const SHADER_INFO& Get_ShaderInfo() const { return m_ShaderInfo; }

public:
	void		Apply();
private:
	SHADER_INFO			m_ShaderInfo;
	CInputLayout*		m_pInputLayout = { nullptr };

private:
	ComPtr<ID3D11Device>		m_pDevice = { nullptr };
	ComPtr<ID3D11DeviceContext>	m_pDeviceContext = { nullptr };


};

NS_END


#pragma once
#include "CComponent.h"
#include "CInputLayout.h"
#include "CBase.h"

NS_BEGIN(Engine)


class ENGINE_DLL CShader final :
                public CBase
{
public:
	typedef struct tagShaderPass
	{
		string Name;		//패스이름
		
		ComPtr<ID3DX11EffectPass> pPass;
		CInputLayout* pInputLayout=nullptr;
	}ShaderPass;


	typedef struct tagCShaderInfo
	{
		ComPtr<ID3DX11Effect>			m_pEffect = { nullptr };
		
		string                m_strTechniqueName = "";
		ComPtr<ID3DX11EffectTechnique>	m_pTechnique = { nullptr };

		//technique에 존재하는 모든pass들을 key값으로 모두 저장하기
		//ShaderPass -> pass,inputlayout묶어놓은것들
		unordered_map<string, tagShaderPass>		m_pPassInfos;


		unsigned int		iPassCnt = 0;

		ComPtr<ID3DBlob>		m_pErrorBlob = { nullptr };
		
		
		ComPtr<ID3DX11EffectMatrixVariable>	m_GlobalViewProj;

		


		tagCShaderInfo() {}
		tagCShaderInfo(const tagCShaderInfo& Prototype)
			:m_pEffect{Prototype.m_pEffect},m_pErrorBlob{Prototype.m_pErrorBlob},m_pTechnique{Prototype.m_pTechnique},
			m_pPassInfos{Prototype.m_pPassInfos }
		{
			m_strTechniqueName = Prototype.m_strTechniqueName;
			
		}




	}SHADER_INFO;

private:
	CShader(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CShader(const CShader& Prototype);
	virtual ~CShader() = default;

public:
	HRESULT     Initialize_Prototype(const vector<D3D11_INPUT_ELEMENT_DESC>& layout, const wstring& filePath, const string& strTechName);
	HRESULT     Initialize_Copytype(void *pArg);

public:
	static CShader* Create(ComPtr<ID3D11Device> pDevice,
		ComPtr<ID3D11DeviceContext> pContext,
		const vector<D3D11_INPUT_ELEMENT_DESC>& layout,
		const wstring& filePath,
		const string& strTechName);
	virtual CShader* Clone(void* pArg);
	virtual void Free() override;


private:
	/*초기화 및 설정 함수들*/
	HRESULT		LoadShaderFromFile(const wstring & path);
	HRESULT		Set_Technique(const string& strTechName);

	//읽어온 Technique에대한 pass들을 모두 저장하고, 이에따른 inputlayout을 만든다.
	HRESULT		Load_PassesAndCreateInputLayers(const vector<D3D11_INPUT_ELEMENT_DESC>& layout);
public:
	HRESULT		Bind_Matrix(const string& Variable, const _float4x4& mat);
	HRESULT		Bind_Vector(const string& Variable, const _float4& vector);
	HRESULT		Bind_Float(const string& Variable, const _float fValue);
	HRESULT		Bind_SRV(const string& Variable, ComPtr<ID3D11ShaderResourceView>& resourc);
	HRESULT		Bind_Sampler(const string& Variable, ComPtr< ID3D11SamplerState> sampler, UINT iIdx = 0);


	HRESULT		Bind_RawFloat(const string& Variable, const void* pData, UINT iSize);
public:
	const SHADER_INFO& Get_ShaderInfo() const { return m_ShaderInfo; }

public:
	HRESULT		Begin(const string& _passName);
private:
	SHADER_INFO			m_ShaderInfo;

private:
	ComPtr<ID3D11Device>		m_pDevice = { nullptr };
	ComPtr<ID3D11DeviceContext>	m_pDeviceContext = { nullptr };


};

NS_END


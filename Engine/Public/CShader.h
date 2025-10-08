#pragma once
#include "CComponent.h"
#include "CBase.h"

NS_BEGIN(Engine)
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
	HRESULT     Initialize_Prototype(const wstring& filePath, const string& strTechName, const string& passName);
	HRESULT     Initialize_Copytype(void *pArg);

public:
	static CShader* Create(ComPtr<ID3D11Device> pDevice,
		ComPtr<ID3D11DeviceContext> pContext,
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


private:
	SHADER_INFO			m_ShaderInfo;

private:
	ComPtr<ID3D11Device>		m_pDevice = { nullptr };
	ComPtr<ID3D11DeviceContext>	m_pDeviceContext = { nullptr };


};

NS_END


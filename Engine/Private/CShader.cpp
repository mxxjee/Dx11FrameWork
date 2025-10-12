#include "CShader.h"
#include "CInputLayout.h"

CShader::CShader(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :m_pDevice{ pDevice }, m_pDeviceContext{pContext}
{
}

CShader::CShader(const CShader& Prototype)
    : m_pDevice{ Prototype.m_pDevice }, m_pDeviceContext{Prototype.m_pDeviceContext},
    m_ShaderInfo{Prototype.m_ShaderInfo},m_pInputLayout{Prototype.m_pInputLayout}
{

}

HRESULT CShader::Initialize_Prototype(const vector<D3D11_INPUT_ELEMENT_DESC>& layout,const wstring& filePath, const string& strTechName, const string& passName)
{
    //1. 셰이더 파일 로드 및 Effect객체 생성
    if(FAILED(LoadShaderFromFile(filePath)))
        return E_FAIL;

    //2. Technique/Pass캐싱
    if(FAILED(Set_Technique(strTechName)))
        return E_FAIL;

    if (FAILED(Set_Pass(passName)))
        return E_FAIL;


    //3. InputLayout생성
    if (FAILED(Create_InputLayout(layout)))
        return E_FAIL;

    m_ShaderInfo.m_GlobalViewProj = m_ShaderInfo.m_pEffect->GetVariableByName("g_ViewProjMatrix")->AsMatrix();
    return S_OK;
}

HRESULT CShader::Initialize_Copytype(void* pArg)
{
    return S_OK;
}

CShader* CShader::Create(ComPtr<ID3D11Device> pDevice,
    ComPtr<ID3D11DeviceContext> pContext,
    const vector<D3D11_INPUT_ELEMENT_DESC>& layout,
    const wstring& filePath,
    const string& strTechName,
    const string& passName)
{
    CShader* pInstance = new CShader(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype(layout,filePath,strTechName,passName)))
    {
        MSG_BOX("Failed to Created : CShader");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CShader* CShader::Clone(void* pArg)
{
    CShader* pInstance = new CShader(*this);

    if (FAILED(pInstance->Initialize_Copytype(pArg)))
    {
        MSG_BOX("Failed to Created : CShader");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CShader::Free()
{
    __super::Free();
    Safe_Release(m_pInputLayout);

}

HRESULT CShader::LoadShaderFromFile(const wstring& path)
{
   
    //일반 hlsl함수 컴파일함수와는 달리 compileFlag를 지정안해도됨, technique랑 pass가 내부적으로 수행중
    
    HRESULT hr = D3DX11CompileEffectFromFile(
        path.c_str(),
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        0,//D3DCOMPILE_ENABLE_STRICTNESS,
        0,
        m_pDevice.Get(),
        m_ShaderInfo.m_pEffect.GetAddressOf(),
        m_ShaderInfo.m_pErrorBlob.GetAddressOf());
    
    /*실패시 문자열 디버그출력*/
    if (FAILED(hr))
    {
        if (m_ShaderInfo.m_pErrorBlob)
            OutputDebugStringA((char*)m_ShaderInfo.m_pErrorBlob->GetBufferPointer());
        return hr;
    }

    return hr;
}

HRESULT CShader::Set_Technique(const string& strTechName)
{
    m_ShaderInfo.m_strTechniqueName = strTechName;
    m_ShaderInfo.m_pTechnique= m_ShaderInfo.m_pEffect->GetTechniqueByName(strTechName.c_str());
    
    /*예외처리.*/
    if (!m_ShaderInfo.m_pTechnique->IsValid())
    {
        OutputDebugStringA(("Technique not found: " + std::string(strTechName.begin(), strTechName.end()) + "\n").c_str());
        return E_FAIL;
    }

    // 기본 pass 0번 설정
    m_ShaderInfo.m_pPass = m_ShaderInfo.m_pTechnique->GetPassByIndex(0);
    return S_OK;
}

HRESULT CShader::Set_Pass(const string& strPassName)
{
    m_ShaderInfo.m_strPassName = strPassName;
    CheckTrueResult(strPassName == "",E_FAIL);

    //strPass 따로지정하지 않았다면  multiplass
    if (!m_ShaderInfo.m_pTechnique)
        return E_FAIL;


    m_ShaderInfo.m_pPass = m_ShaderInfo.m_pTechnique->GetPassByName(strPassName.c_str());
    if (!m_ShaderInfo.m_pPass->IsValid())
    {
        OutputDebugStringA(("Pass not found: " + std::string(strPassName.begin(), strPassName.end()) + "\n").c_str());
        return E_FAIL;
    }

    return S_OK;
}

HRESULT CShader::Create_InputLayout(const vector<D3D11_INPUT_ELEMENT_DESC>& layout)
{
    D3DX11_PASS_DESC  passDesc{};
    m_ShaderInfo.m_pPass->GetDesc(&passDesc);

    m_pInputLayout = CInputLayout::Create(m_pDevice, m_pDeviceContext, 
        layout, layout.size(),
        passDesc.pIAInputSignature, passDesc.IAInputSignatureSize);

    return S_OK;
}

void CShader::SetMatrix(const string& Variable, const _float4x4& mat)
{
    
    m_ShaderInfo.m_pEffect->GetVariableByName(Variable.c_str())->AsMatrix()->SetMatrix((float*)mat.m);

}

void CShader::SetVector(const string& Variable, const _float4& vector)
{
    m_ShaderInfo.m_pEffect->GetVariableByName(Variable.c_str())->AsVector()->SetFloatVector((float*)(&vector));
}

void CShader::SetFloat(const string& Variable, const _float fValue)
{
    m_ShaderInfo.m_pEffect->GetVariableByName(Variable.c_str())->AsScalar()->SetFloat(fValue);
}

void CShader::SetResource(const string& Variable, ComPtr<ID3D11ShaderResourceView> resource)
{
    m_ShaderInfo.m_pEffect->GetVariableByName(Variable.c_str())->AsShaderResource()->SetResource(resource.Get());
    
}

void CShader::SetSampler(const string& Variable, ComPtr<ID3D11SamplerState> sampler,UINT iIdx)
{
    m_ShaderInfo.m_pEffect->GetVariableByName(Variable.c_str())->AsSampler()->SetSampler(iIdx,sampler.Get());
}

void CShader::Apply()
{
    if(m_pInputLayout)
        m_pInputLayout->Set_InputLayout();


    //내부적으로 Context->VSSetShader() / Context-PSSetShader()같은 함수를 알아서호출해줌
    // 이 함수는 상수버퍼값 전달 이후 불려야한다.
    m_ShaderInfo.m_pPass->Apply(0, m_pDeviceContext.Get());

}

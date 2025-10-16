#include "CShader.h"


CShader::CShader(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :m_pDevice{ pDevice }, m_pDeviceContext{pContext}
{
}

CShader::CShader(const CShader& Prototype)
    : m_pDevice{ Prototype.m_pDevice }, m_pDeviceContext{Prototype.m_pDeviceContext},
    m_ShaderInfo{Prototype.m_ShaderInfo}
{

}

HRESULT CShader::Initialize_Prototype(const vector<D3D11_INPUT_ELEMENT_DESC>& layout,
    const wstring& filePath,
    const string& strTechName)
{
    //1. 셰이더 파일 로드 및 Effect객체 생성
    if(FAILED(LoadShaderFromFile(filePath)))
        return E_FAIL;

    //2. Technique/Pass캐싱
    if(FAILED(Set_Technique(strTechName)))
        return E_FAIL;

    //Pass 캐싱하고 inputlayout들 생성
    if(FAILED(Load_PassesAndCreateInputLayers(layout)))
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
    const string& strTechName)
{
    CShader* pInstance = new CShader(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype(layout,filePath,strTechName)))
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
    for (auto& pair : m_ShaderInfo.m_pPassInfos)
    {
        Safe_Release(pair.second.pInputLayout);
    }

}

HRESULT CShader::LoadShaderFromFile(const wstring& path)
{
   
    //일반 hlsl함수 컴파일함수와는 달리 compileFlag를 지정안해도됨, technique랑 pass가 내부적으로 수행중
    
    _uint iHlslFlag = {};

#ifdef  _DEBUG
    iHlslFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;

#else
    iHlslFlag = D3DCOMPILE_OPTIMIZATION_LEVEL1;

#endif //  _DEBUG


    HRESULT hr = D3DX11CompileEffectFromFile(
        path.c_str(),
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,  //셰이더에서 include쓸수있는 상태
        iHlslFlag,      //D3DCOMPILE_ENABLE_STRICTNESS,
        0,              //VS or PS인지, 하지만 우리는 한번에 다 넣어서 0넣어줘도됨
        m_pDevice.Get(),    
        m_ShaderInfo.m_pEffect.GetAddressOf(),          //결과 effect객체 
        m_ShaderInfo.m_pErrorBlob.GetAddressOf());      //blob(메모리덩어리)-> 개발자에게 보여주고싶은 메모리값들.? 
                                                        //blob은 함수에 따라서 담겨지는게 다르다. 
                                                         //이 함수에서는 문자열을 담아준다. 
    
                                                        
                                                        
     /*실패시 문자열 디버그출력*/
 /*   if (FAILED(hr))
    {
        if (m_ShaderInfo.m_pErrorBlob)
            OutputDebugStringA((char*)m_ShaderInfo.m_pErrorBlob->GetBufferPointer());
        return hr;
    }*/

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

    return S_OK;
}

HRESULT CShader::Load_PassesAndCreateInputLayers(const vector<D3D11_INPUT_ELEMENT_DESC>& layout)
{
    CheckNullResult(m_ShaderInfo.m_pTechnique,E_FAIL);
    
    D3DX11_TECHNIQUE_DESC TechniqueDesc{};
    
    m_ShaderInfo.m_pTechnique->GetDesc(&TechniqueDesc);
    m_ShaderInfo.iPassCnt = TechniqueDesc.Passes;

    for (size_t i = 0; i < m_ShaderInfo.iPassCnt; ++i)
    {
        /*UMap 채워주기*/
        ShaderPass PassInfo;

        ComPtr<ID3DX11EffectPass> pPass = m_ShaderInfo.m_pTechnique->GetPassByIndex(i);
        D3DX11_PASS_DESC        PassDesc = {};
        pPass->GetDesc(&PassDesc);

        CInputLayout* pInputLayout = CInputLayout::Create(m_pDevice, m_pDeviceContext,
            layout, layout.size(),
            PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize);

        CheckNullResult(pInputLayout, E_FAIL);

        PassInfo.Name = PassDesc.Name;
        PassInfo.pPass = pPass;
        PassInfo.pInputLayout = pInputLayout;

        m_ShaderInfo.m_pPassInfos.emplace(PassInfo.Name, PassInfo);

    }



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

void CShader::SetResource(const string& Variable, ComPtr<ID3D11ShaderResourceView>& resource)
{
    m_ShaderInfo.m_pEffect->GetVariableByName(Variable.c_str())->AsShaderResource()->SetResource(resource.Get());
    
}

void CShader::SetSampler(const string& Variable, ComPtr<ID3D11SamplerState> sampler,UINT iIdx)
{
    m_ShaderInfo.m_pEffect->GetVariableByName(Variable.c_str())->AsSampler()->SetSampler(iIdx,sampler.Get());
}

HRESULT CShader::Begin(const string& _passName)
{
    auto iter = m_ShaderInfo.m_pPassInfos.find(_passName);
    if (iter == m_ShaderInfo.m_pPassInfos.end())
        return E_FAIL;

        
    CInputLayout* pInputLayout = iter->second.pInputLayout;
    CheckNullResult(pInputLayout,E_FAIL);
    pInputLayout->Set_InputLayout();


    //내부적으로 Context->VSSetShader() / Context-PSSetShader()같은 함수를 알아서호출해줌
    // 이 함수는 상수버퍼값 전달 이후 불려야한다.

    //이 패스로 그릴거라고 알려주는 행위
    iter->second.pPass->Apply(0, m_pDeviceContext.Get());
    return S_OK;
}

#pragma once
#include "CBase.h"

NS_BEGIN(Engine)
class CShader;

class CShader_Manager :
    public CBase
{
private:
    CShader_Manager(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pContext);
    virtual ~CShader_Manager() = default;
public:
     HRESULT        Initialize();
     HRESULT       Register_Shader(const _wstring& Tag,CShader* pInstance);

public:
    static CShader_Manager*     Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pContext);
    virtual void                Free() override;

public:
    //모든 셰이더파일들에게 전역변수들을 바인딩(카메라 뷰관련,, 수치등)
    HRESULT         Bind_GlobalPipelineData(_uint CameraType);

    //모든 셰이더파일들에게 LIGHT값 바인딩
    HRESULT         Bind_GlobalLightData();

    //모든 셰이더파일들에게 Sampler값 바인딩
    HRESULT         Bind_SamplerState(_uint iRenderGroup);
public:
    CShader*                Find_Shader(const _wstring& Tag);

private:
    UMap<_wstring, CShader*>    m_mapShader;

private:
    ComPtr<ID3D11Device> m_pDevice = { nullptr };
    ComPtr<ID3D11DeviceContext> m_pContext = { nullptr };

};
NS_END


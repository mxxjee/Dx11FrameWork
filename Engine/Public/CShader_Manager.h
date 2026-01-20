#pragma once
#include "CBase.h"
#include "ConstantStruct.h"

NS_BEGIN(Engine)
class CShader;
class CConstantBuffer;

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
    /*셰이더 파일에게 상수버퍼 연결시켜주는 함수*/
    HRESULT         Set_ConstantBuffer(CShader* pInstance);
    

    
    //모든 셰이더파일들에게 LIGHT값 바인딩
    HRESULT         Bind_GlobalLightData();

    //모든 셰이더파일들에게 Sampler값 바인딩
    HRESULT         Bind_SamplerState(_uint iRenderGroup);

    //상수버퍼 갱신용
    void            CopyData_Buffer(string Key, const void* pData,_uint iSize);


public:
    CShader*                Find_Shader(const _wstring& Tag);

private:
    UMap<_wstring, CShader*>    m_mapShader;

private:
    ComPtr<ID3D11Device> m_pDevice = { nullptr };
    ComPtr<ID3D11DeviceContext> m_pContext = { nullptr };

private:
    HRESULT         Create_GlobalConstantBuffer();

private:
    //전역 상수버퍼들(세팅할 값)
    UMap<string, CConstantBuffer*>  m_ConstantBuffers;

  



};
NS_END


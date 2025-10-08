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
    CShader*                Find_Shader(const _wstring& Tag);

private:
    UMap<_wstring, CShader*>    m_mapShader;

private:
    ComPtr<ID3D11Device> m_pDevice = { nullptr };
    ComPtr<ID3D11DeviceContext> m_pContext = { nullptr };

};
NS_END


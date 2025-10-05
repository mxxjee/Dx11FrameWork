#pragma once
#include "CComponent.h"

NS_BEGIN(Engine)
class ENGINE_DLL CShader final :
    public CComponent
    {
     private:
        CShader(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
        CShader(const CShader& Prototype);
        virtual ~CShader() = default;


    public:
        virtual         HRESULT     Initialize_Prototype(const _tchar * pShaderFilePath);
        virtual         HRESULT     Initialize_Copytype(void* pArg) override;
private:
    ComPtr<ID3DX11Effect> m_pEffect = { nullptr };
public:
    static CShader* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const _tchar * pShaderFilePath);
    virtual CComponent* Clone(void* pArg);
    virtual void Free() override;

};

NS_END


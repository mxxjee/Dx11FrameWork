#pragma once
#include "CBase.h"
#include "ModelData.h"

NS_BEGIN(Engine)
class CTexture;
class CGameInstance;
class ENGINE_DLL CMaterial :
    public CBase
{

public:
    CMaterial(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pContext);
    CMaterial(const CMaterial& Prototype);
    ~CMaterial() = default;

public:
    virtual HRESULT Initialize_Prototype(const char* BasePath, map<aiTextureType, string>& _TextureData);
    void            Set_Name(const wstring& pDst) { m_MatData.m_MaterialName = pDst; }

private:
    HRESULT         Register_MaterialTexture(const char* BasePath, map<aiTextureType, string>& _TextureData);
public:
    static CMaterial* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext,const char* BasePath, map<aiTextureType, string>& _TextureData);
    virtual void Free() override;

public:
    CTexture* Get_Texture(aiTextureType eType);

    HRESULT         Bind_ShaderResource(class CShader* pShader, const string& Variable,aiTextureType eType, int idx=0);
    const MaterialData* Get_MaterialData() { return &m_MatData; }
private:
    MaterialData            m_MatData;

     
private:
    ComPtr<ID3D11Device>					m_pDevice;
    ComPtr<ID3D11DeviceContext>				m_pContext;


private:
    CGameInstance* m_pGameInstance = nullptr;

};
NS_END


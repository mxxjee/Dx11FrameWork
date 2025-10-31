#pragma once
#include "CComponent.h"
#include "ModelData.h"


NS_BEGIN(Engine)
class CVIBuffer_Model;
class CMaterial;
class CShader;
class CGameInstance;


class ENGINE_DLL CMeshComponent :
    public CComponent
{

protected:
    CMeshComponent(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CMeshComponent(const CMeshComponent& Prototype);
    virtual ~CMeshComponent() = default;

public:
    virtual HRESULT Initialize_Prototype(const MeshData& Data, const char* BasePath, _uint iIdx);
    virtual HRESULT Initialize_Copytype(void* pArg) override;

private:
    bool        LoadBinaryVB(const string& Path);
    bool        LoadBinaryIB(const string& Path);
    HRESULT        Set_Material();

public:
    static CMeshComponent* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const MeshData& Data,  const char* BasePath, _uint iIdx);
    virtual CComponent* Clone(void* pArg) override;
    virtual void Free() override;

    HRESULT                 Render();


public:
    HRESULT         Bind_ShaderResource(CShader*  pShader, const _char* pConstName, aiTextureType eMaterialType, _uint Textureindex=0);
    const string& Get_PassName() { return passName; }
public:
    void            Set_PassName(const string& Name) { passName = Name; }
private:
    MeshData                m_MeshData;
    CVIBuffer_Model*        m_pVIBuffer;
    CMaterial*              m_pMaterial = nullptr;

    string              passName = "Default";


    CGameInstance*      m_pGameInstance = nullptr;


};

NS_END

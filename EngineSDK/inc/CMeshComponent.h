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
    virtual HRESULT Initialize_Prototype(const MeshData& Data, const char* BasePath, _uint iIdx, MODEL eModelType);
    virtual HRESULT Initialize_Copytype(void* pArg) override;

private:

    HRESULT        Set_Material();

private:
    HRESULT Ready_For_NonAnimMesh(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, _matrix& Matrix, vector<VTXMESH>& Vertices, vector<_uint>& Indices);
    HRESULT Ready_For_AnimMesh(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, _matrix& Matrix, vector<VTXMESH>& Vertices, vector<_uint>& Indices);

public:
    static CMeshComponent* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const MeshData& Data,  const char* BasePath, _uint iIdx, MODEL eModelType);
    virtual CComponent* Clone(void* pArg) override;
    virtual void Free() override;

    HRESULT                 Render();

public:
    bool                Intersects_Ray(_vector origin, _vector rayDir, _float& Dist);

public:
    HRESULT         Bind_ShaderResource(CShader*  pShader, const _char* pConstName, aiTextureType eMaterialType, _uint Textureindex=0);
    const string& Get_PassName() { return passName; }
    const MeshData& Get_MeshData() { return m_MeshData; }
public:
    void            Set_PassName(const string& Name) { passName = Name; }
private:
    MeshData                m_MeshData;
    CVIBuffer_Model*              m_pVIBuffer;
    CMaterial*              m_pMaterial = nullptr;

    string              passName = "Default";

private:
    CGameInstance*      m_pGameInstance = nullptr;

    vector<_uint>       m_pIndices;
    vector<_float3>     m_pPositions;
};

NS_END

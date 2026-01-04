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

public:
    static CMeshComponent* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const MeshData& Data,  const char* BasePath, _uint iIdx,MODEL eModelType);
    virtual CComponent* Clone(void* pArg) override;
    virtual void Free() override;

    HRESULT                 Render();
    bool                Has_aiTexture(aiTextureType eType);
  
public:
    bool                Intersects_Ray(_vector origin, _vector rayDir, _float& Dist);
    void                Set_Visible(bool bVisible) { m_bVisible = bVisible; }

public:
    HRESULT         Bind_ShaderResource(CShader*  pShader, const _char* pConstName, aiTextureType eMaterialType, _uint Textureindex=0);
    HRESULT         Bind_Bones(CShader* pShader, const _char* pConstatName, const vector<class CBone*>& Bones);//쉐이더에게 현재 이 메쉬가 영향받는 본들의 행렬으 넘겨준다.

    const string& Get_PassName() { return passName; }
    const MeshData& Get_MeshData() { return m_MeshData; }
public:
    void            Set_PassName(const string& Name) { passName = Name; }
private:
    CVIBuffer_Model*              m_pVIBuffer;
    CMaterial*              m_pMaterial = nullptr;
    string              passName = "Default";

private:
    MeshData                m_MeshData;
    //이 메쉬에게 영향을 주는 본의 combinedtransform을 모아놓는곳.
    _float4x4           m_BoneMatrices[g_iMaxNumBones];


private:
    CGameInstance*      m_pGameInstance = nullptr;

    vector<_uint>       m_pIndices;
    vector<_float3>     m_pPositions;

    bool                m_bVisible = true;
};

NS_END

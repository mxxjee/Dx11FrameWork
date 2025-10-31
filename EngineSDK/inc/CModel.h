#pragma once
#include "CComponent.h"
#include "ModelData.h"

NS_BEGIN(Engine)

class CMeshComponent;
class CGameInstance;
class CShader;

/*모델이 공통적인 셰이더를 가지고....
각 메쉬는 pass만다르게가진다..? */

class ENGINE_DLL CModel :
    public CComponent
{
public:
    typedef struct tagModelDesc
    {
        _wstring ShaderName = L"VtxMesh";
        string passName = "Default";

    }MODEL_DSC;
protected:
    CModel(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CModel(const CModel& Prototype);
    virtual ~CModel() = default;

public:
    virtual HRESULT Initialize_Prototype(_matrix PreTransformMatrix,const _char* pFilePath);
    virtual HRESULT Initialize_Copytype(void* pArg) override;



private:
    HRESULT     LoadModelFromJson(_matrix PreTransformMatrix,const _char* filepPath);
    HRESULT     LoadMaterialFromJSon(const _char* filePath);

public:
        //모든 메쉬 ,메테리얼바인딩 하나씩 한 이후 호출하는 그리기작업 수행함수
    HRESULT     Render(CMeshComponent*   pMesh);  
public:

    static CModel* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext,_matrix PreTransformMatrix,const _char* pFilePath);
    virtual CComponent* Clone(void* pArg) override;
    virtual void Free() override;

public:
    HRESULT             Bind_Mateiral(CShader* pShader, const _char* pConstName, CMeshComponent* pMesh, aiTextureType eMaterialType, _uint Textureindex = 0);


public:
    CMeshComponent* Get_Mesh(const wstring& Name);
    const UMap<wstring, CMeshComponent*>& Get_Meshs() { return m_Meshs; }
    class CShader* Get_Shader() { return m_pShader; }
private:
    UMap<wstring, CMeshComponent*>      m_Meshs;
    ModelData       m_ModelData;
    CShader* m_pShader = nullptr;

private:
    CGameInstance* m_pGameInstance = nullptr;
};
NS_END

#pragma once
#include "CBase.h"
#include "ModelData.h"

NS_BEGIN(Engine)

class CMeshComponent;
class CGameInstance;
class CShader;
class CBone;

/*모델이 공통적인 셰이더를 가지고....
각 메쉬는 pass만다르게가진다..? */

class ENGINE_DLL CModel :
    public CBase
{
public:
    typedef struct tagModelDesc
    {
        _wstring ShaderName = L"VtxMesh";
        string passName = "Default";
        class CGameObject* pOwner = nullptr;

    }MODEL_DSC;
protected:
    CModel(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CModel(const CModel& Prototype);
    virtual ~CModel() = default;

public:
    HRESULT Initialize_Prototype(_matrix PreTransformMatrix,const _char* pFilePath);
    HRESULT Initialize_Copytype(void* pArg);
    

private:
    HRESULT     LoadModelFromJson(bool _bHasAnim,_matrix PreTransformMatrix,const _char* filepPath,json& jModel);
    HRESULT     LoadMaterialFromJSon(const _char* filePath,json& jModel);
    HRESULT     Load_BonesFromJson(json& jModel);
    HRESULT     Load_Animation(const _char* filePath);

public:
        //모든 메쉬 ,메테리얼바인딩 하나씩 한 이후 호출하는 그리기작업 수행함수
    HRESULT     Render(CMeshComponent*   pMesh);  
public:

    static CModel* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext,_matrix PreTransformMatrix,const _char* pFilePath);
    virtual CModel* Clone(void* pArg);
    virtual void Free() override;

public:
    HRESULT             Bind_Mateiral(CShader* pShader, const _char* pConstName, CMeshComponent* pMesh, aiTextureType eMaterialType, _uint Textureindex = 0);
    HRESULT             Bind_Bones(CShader* pShader, const char* pConstName,CMeshComponent* pMesh);
    void                Play_Animation(_float fTimeDelta);

public:
    /*레이충돌*/
    bool        Intersects_Ray(_vector origin, _vector rayDir, _float& Dist);
    void        Set_Animation(_uint iAnimationIndex, _bool isLoop);
    int         Get_BoneIndex(const char* pBoneName);
public:
    CMeshComponent* Get_Mesh(const wstring& Name);
    const UMap<wstring, CMeshComponent*>& Get_Meshs() { return m_Meshs; }
    class CShader* Get_Shader() { return m_pShader; }
    const ModelData& Get_ModelData() { return m_ModelData; }
private:
    UMap<wstring, CMeshComponent*>          m_Meshs;
    vector<class CBone*>                    m_Bones;

private:
    ModelData                           m_ModelData;
    CShader*                            m_pShader = nullptr;
    MODEL					            m_eModelType = {};

    _float4x4                           m_PreTransformMatrix = {};


    _uint                               m_iNumAnimations = {};  //애니메이션 개수
    _uint                               m_iCurrentAnimIndex = {};       //현재 재생되고있는 애니메이션 클립의 인덱스
    vector<class CAnimation*>           m_Animations;
    bool                                m_isAnimFinished = false;
private:
    CGameInstance* m_pGameInstance = nullptr;
    ComPtr<ID3D11Device>                m_pDevice;
    ComPtr<ID3D11DeviceContext>         m_pDeviceContext;

private:
    class CGameObject*              m_pOwner = nullptr;
};
NS_END

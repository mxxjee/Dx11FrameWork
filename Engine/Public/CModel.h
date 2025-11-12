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
    void                Update_BlendAnim(_float fTimeDelta);
    void                Set_TransitionTime(_float fTime) { m_fTranslation = fTime; }
public:
    /*레이충돌*/
    bool        Intersects_Ray(_vector origin, _vector rayDir, _float& Dist);
    void        Set_Animation(const wstring& AnimKey, _bool isLoop);
    int         Get_BoneIndex(const char* pBoneName);
    int         Get_NumAnim() { return m_iNumAnimations; }
    void        Set_Loop(const wstring& AnimKey, bool bLoop);

public:
    CMeshComponent* Get_Mesh(const wstring& Name);

    const UMap<wstring, CMeshComponent*>& Get_Meshs() { return m_Meshs; }
    const UMap<wstring, class CAnimation*>& Get_Anims()   { return m_Animations; }

    
    class CShader* Get_Shader() { return m_pShader; }
    const ModelData& Get_ModelData() { return m_ModelData; }
    class CAnimation* Find_Animation(const _wstring& Key);

public:
    void        Set_VisibleMesh(const _wstring& MeshName,bool bVisible);

public:
    void            Start_Transition();
    void            End_Transition();
private:
    UMap<wstring, CMeshComponent*>          m_Meshs;
    vector<class CBone*>                    m_Bones;

private:
    ModelData                           m_ModelData;
    CShader*                            m_pShader = nullptr;
    MODEL					            m_eModelType = {};

    _float4x4                           m_PreTransformMatrix = {};


    _uint                               m_iNumAnimations = {};  //애니메이션 개수
    
    wstring                               m_CurrentAnimKey = L"";       //현재 재생되고있는 애니메이션 클립의 인덱스
    wstring                               m_PreAnimKey= L"";


    UMap<wstring,class CAnimation*>           m_Animations;
    vector<_matrix>                   m_BoneTraformMatricies;

    bool                                m_isAnimFinished = false;
    //전이 확인
    bool                                m_isTransition = false;

    _float                              m_fTranslationTime = 0.f;
    _float                              m_fTranslation = 0.2f;
private:
    CGameInstance* m_pGameInstance = nullptr;
    ComPtr<ID3D11Device>                m_pDevice;
    ComPtr<ID3D11DeviceContext>         m_pDeviceContext;

private:
    class CGameObject*              m_pOwner = nullptr;
};
NS_END

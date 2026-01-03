#pragma once
#include "CGameObject.h"
#include "EffectData.h"

namespace Engine
{
    class CModel;
    class CShader;
}

NS_BEGIN(Client)

class CEffectData_Manager;

class CMeshEffect :
    public CGameObject
{
public:
    typedef struct tagMeshEffectDesc : public CGameObject::tagGameObjectDesc
    {
        wstring    modelName;               //이 body가 사용할 모델이름
        wstring      ShaderName=L"";
        string      PassName="Default";
        _uint eRenderGroup = 0;

    }MESHEFFECT_DESC;

#ifdef _DEBUG
public:
    virtual void        Render_DebugImgui();
#endif // _DEBUG

protected:
    CMeshEffect(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CMeshEffect(const CMeshEffect& rhs);
    virtual ~CMeshEffect() = default;
    
public:
    virtual HRESULT     Initialize_Prototype(); /*원형 객체가 생성될때 부르는 Initialize*/
    virtual HRESULT     Initialize_Copytype(void* pArg); /*원형 객체가 생성될때 부르는 Initialize*/

    virtual void        Update_Priority(_float fTimeDelta);
    virtual void        Update(_float fTimeDelta);
    virtual void        Update_Late(_float fTimeDelta);
    virtual void        Update_Render(_float fTimeDelta) override;


    virtual HRESULT Render();

public:
    void Set_VisibleMesh(const wstring& MeshName, bool bVisible);
    const EffectData* Get_EffectData() { return &m_LocalData; }

protected:
    CModel* m_pModel = { nullptr };
    //모델이 사용하는 쉐이더
    CShader* m_pShader = { nullptr };

public:
    CModel* Get_Model() { return m_pModel; }
    CShader* Get_Shader() { return m_pShader; }

protected:
    virtual HRESULT         Ready_Components(void* pArg);
    virtual HRESULT         Ready_Resource(void* pArg);

    virtual HRESULT         Bind_ShaderResources();


public:
    static CMeshEffect* Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;

public:

    void    Spawn(const _float4x4* pSocketMatrix = nullptr,const _float4x4* pParentMatrix=nullptr);


    void    Play();
    void     Stop();
protected:
    _uint                   m_eRenderGroup = 0;
    string                  m_PassName = "";
public:
    EffectData              m_LocalData;
    float                   m_fTime = 0.f;
    wstring                 m_ModelName = L"";

    CEffectData_Manager* m_pEffectData_Manager = nullptr;
    bool                m_bStop = false;

private:
 //   void Update_Matrix();


private:
    _float4x4   m_CombinedWorldMatrix;
    const _float4x4* m_pSocketMatrix = { nullptr };
    const _float4x4* m_pParentMatrix = { nullptr };


};
NS_END


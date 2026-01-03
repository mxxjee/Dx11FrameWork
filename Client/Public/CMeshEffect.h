#pragma once
#include "CGameObject.h"
#include "EffectData.h"
#include "CEffect.h"

namespace Engine
{
    class CModel;
    class CShader;
}

NS_BEGIN(Client)

class CEffectData_Manager;

class CMeshEffect :
    public CEffect
{
public:
    typedef struct tagMeshEffectDesc : public CEffect::tagEffectDesc
    {
        wstring    modelName;               //이 body가 사용할 모델이름
        string      PassName="Default";
       
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
  
protected:
    CModel* m_pModel = { nullptr };

public:
    CModel* Get_Model() { return m_pModel; }

protected:
    virtual HRESULT         Ready_Components(void* pArg);
    virtual HRESULT         Ready_Resource(void* pArg);

    virtual HRESULT         Bind_ShaderResources();


public:
    static CMeshEffect* Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;

public:
    virtual void    Spawn(const _float4x4* pSocketMatrix = nullptr,const _float4x4* pParentMatrix=nullptr);
    virtual void    Play();
    virtual void     Stop();


protected:
    _uint                   m_eRenderGroup = 0;
    string                  m_PassName = "";
    

public:
    EffectData              m_LocalData;
    float                   m_fTime = 0.f;
    wstring                 m_ModelName = L"";

  


};
NS_END


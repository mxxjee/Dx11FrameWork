#pragma once
#include "CGameObject.h"

NS_BEGIN(Engine)
class CModel;

class ENGINE_DLL CModelObject :
    public CGameObject
{
public:
    typedef struct tagModelObjectDesc : CGameObject::GAMEOBJECT_DESC
    {
        _wstring    modelName;
        _wstring    ShaderName = L"VtxMesh";
        string      passName = "Default";

        _uint eRenderGroup = 0;
    }MODELOBJECT_DESC;

protected:
    CModelObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CModelObject(const CModelObject& rhs);
    virtual ~CModelObject() = default;
    

public:
    virtual HRESULT     Initialize_Prototype(); /*원형 객체가 생성될때 부르는 Initialize*/
    virtual HRESULT     Initialize_Copytype(void* pArg); /*사본 객체가 생성될때 부르는 Initialize*/

    virtual void        Update_Priority(_float fTimeDelta);
    virtual void        Update(_float fTimeDelta);
    virtual void        Update_Late(_float fTimeDelta);
    virtual void        Update_Render(_float fTimeDelta);


    virtual HRESULT Render();

private:
    HRESULT                     Bind_ShaderResources();

public:
    static CModelObject* Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;


protected:
    CModel* m_pModelComp = nullptr;
    class CShader* m_pShader = nullptr;
private:
    HRESULT     Ready_Components(void* pArg);
    HRESULT     Ready_Resource(void* pArg);

protected:
    
    _uint                   m_eRenderGroup = 0;
    _wstring                m_ShaderName = L"";
    string                  m_passName = "";

};
NS_END


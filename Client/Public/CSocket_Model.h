#pragma once
#include "CPartObject.h"

namespace Engine
{
    class CModel;
    class CShader;
}



NS_BEGIN(Client)
class CSocket_Model :
    public CPartObject
{
public:
    typedef struct tagSocketModelDesc : public CPartObject::PARTOBJECT_DESC
    {
        wstring ModelName = L"";
        const _float4x4* pSocketMatrix = { nullptr };
    }SOKET_MODEL_DESC;

protected:
    CSocket_Model(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CSocket_Model(const CSocket_Model& Prototype);
    virtual ~CSocket_Model() = default;

public:
    virtual HRESULT     Initialize_Prototype(); /*원형 객체가 생성될때 부르는 Initialize*/
    virtual HRESULT     Initialize_Copytype(void* pArg); /*사본 객체가 생성될때 부르는 Initialize*/

    virtual void        Update_Priority(_float fTimeDelta);
    virtual void        Update(_float fTimeDelta);
    virtual void        Update_Late(_float fTimeDelta);
    virtual void        Update_Render(_float fTimeDelta);

    virtual HRESULT Render();

private:
    HRESULT         Ready_Component(void* pArg);

    HRESULT         Bind_ShaderResources();

public:
    static CSocket_Model* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;

    

protected:
    const _float4x4* m_pSocketMatrix = { nullptr };

private:
    CModel* m_pModel = nullptr;
    //모델이 사용하는 쉐이더
    CShader* m_pShader = { nullptr };

};

NS_END
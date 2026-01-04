#pragma once
#include "CPartObject.h"

/*mesh와 shader를 들고있음.
부모의 상태에 따라 애니메이션을 제어하는 역할.
실제 메쉬를 그려주는 역할
*/

NS_BEGIN(Engine)

class CModel;
class CShader;

class ENGINE_DLL CBody :
    public CPartObject
{
public:
    typedef struct tagBodyDesc : public CPartObject::tagPartObjectDesc
    {
        wstring    modelName;               //이 body가 사용할 모델이름
        _uint eRenderGroup = 0;
    
    }BODY_DESC;


protected:
    CBody(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CBody(const CBody& rhs);
    virtual ~CBody() = default;

public:
    virtual HRESULT     Initialize_Prototype(); /*원형 객체가 생성될때 부르는 Initialize*/
    virtual HRESULT     Initialize_Copytype(void* pArg); /*사본 객체가 생성될때 부르는 Initialize*/

    virtual void        Update_Priority(_float fTimeDelta);
    virtual void        Update(_float fTimeDelta);
    virtual void        Update_Late(_float fTimeDelta);
    virtual void        Update_Render(_float fTimeDelta);

    virtual HRESULT Render();

public:
    void Set_VisibleMesh(const wstring& MeshName, bool bVisible);
    

public:
    _float3             Get_RootDelta();


public:
    const _float4x4* Get_SocketMatrix(const char* pBoneName);


protected:
    CModel* m_pModel = { nullptr };

            //모델이 사용하는 쉐이더
    CShader* m_pShader = { nullptr };

public:
    CModel* Get_Model() { return m_pModel; }
    CShader* Get_Shader() { return m_pShader; }
 


protected:
    virtual HRESULT         Ready_Components(void *pArg);
    virtual HRESULT         Ready_Resource(void* pArg);

    virtual HRESULT         Bind_ShaderResources();

public:
    static CBody* Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;

protected:
    _uint                   m_eRenderGroup = 0;
    bool            m_bHasNormal = true;

};
NS_END


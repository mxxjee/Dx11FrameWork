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
        const _uint* pParentState = nullptr;//부모의 상태를 통해서 애님제어
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
    void Set_Animation_Speed(const wstring& AnimName, _float fSpeed);


public:
    _float3             Get_RootDelta();
protected:
    const _uint* m_pParentState = { nullptr };
    const _uint* m_pParentPreState = { nullptr };

public:
    const _float4x4* Get_SocketMatrix(const char* pBoneName);


protected:
    CModel* m_pModel = { nullptr };

            //모델이 사용하는 쉐이더
    CShader* m_pShader = { nullptr };

public:
    CModel* Get_Model() { return m_pModel; }
    CShader* Get_Shader() { return m_pShader; }
    bool        Get_IsAnimFinished();


protected:
    HRESULT         Ready_Components(void *pArg);
    HRESULT         Ready_Resource(void* pArg);

    virtual HRESULT         Bind_ShaderResources();

public:
    static CBody* Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;

public:
    //Body Update 시 Set_Animation할 애니메이션 설정
    void        Reserve_Animation(_wstring AnimKey, bool bLoop, bool immediately=false);
private:
    _uint                   m_eRenderGroup = 0;


protected:
    wstring             m_NextAnimKey = L"";
    bool                m_NextAnimLoop = false;

    float               m_fInitTransitionTime=0.2f;

};
NS_END


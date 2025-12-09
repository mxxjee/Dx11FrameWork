#pragma once
#include "CBody.h"

NS_BEGIN(Engine)

class CModel;
class CShader;
class ENGINE_DLL CAnimBody :
    public CBody
{
public:
    typedef struct tagBodyDesc : public CBody::tagBodyDesc
    {
        const _uint* pParentState = nullptr;//부모의 상태를 통해서 애님제어

    
    }ANIMBODY_DESC;


protected:
    CAnimBody(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CAnimBody(const CAnimBody& rhs);
    virtual ~CAnimBody() = default;

public:
    virtual HRESULT     Initialize_Prototype(); /*원형 객체가 생성될때 부르는 Initialize*/
    virtual HRESULT     Initialize_Copytype(void* pArg); /*사본 객체가 생성될때 부르는 Initialize*/

    virtual void        Update_Priority(_float fTimeDelta);
    virtual void        Update(_float fTimeDelta);
    virtual void        Update_Late(_float fTimeDelta);
    virtual void        Update_Render(_float fTimeDelta);

    virtual HRESULT Render();


public:
    void Set_Animation_Speed(const wstring& AnimName, _float fSpeed);
    bool        Get_IsAnimFinished();
public:
    static CAnimBody* Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;

public:
    //Body Update 시 Set_Animation할 애니메이션 설정
    void        Reserve_Animation(_wstring AnimKey, bool bLoop, bool immediately = false);
    wstring     Get_CurrentAnimKey();
protected:
    virtual HRESULT         Ready_Components(void* pArg);

protected:
    const _uint* m_pParentState = { nullptr };
    const _uint* m_pParentPreState = { nullptr };

protected:
    wstring             m_NextAnimKey = L"";
    bool                m_NextAnimLoop = false;

    float               m_fInitTransitionTime = 0.2f;

};

NS_END
#pragma once
#include "CComponent.h"

NS_BEGIN(Engine)
class CUI;
class CTransform;

class ENGINE_DLL CUIComponent :
    public CComponent
{
public:
    struct UIAnimInfo
    {
        bool        m_bPlay = false;
        float       m_fSpeed = 1.f;
        bool        m_bEnd = false;
        _float4     fStart;     //시작값
        _float4     fTarget;    //목표값
        bool        bLoop;      //반복여부

    };

    typedef struct tagUICompDesc : CComponent::tagComponentDesc
    {
        UIAnimInfo          _AnimInfo[ENUM_TO_UINT(UIAnimType::COUNT)];

    }UICOMP_DESC;

private:
    CUIComponent(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CUIComponent(const CUIComponent& Prototype);
    virtual ~CUIComponent() = default;

public:
    virtual         HRESULT     Initialize_Prototype() override;
    virtual         HRESULT     Initialize_Copytype(void* pArg) override;

public:
    HRESULT         Update_Component(_float fTimeDelta);

public:
    void            Update_UIAnim(UIAnimType eType, _float fTimeDelta);
    void            LerpAlpha(_float vTarget, _float fLerpSpeed, _float fTimeDelta);

public:
    static CUIComponent* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    virtual CComponent* Clone(void* pArg);
    virtual void Free() override;

private:
    UIAnimInfo          m_AnimInfo[ENUM_TO_UINT(UIAnimType::COUNT)];
    CUI*                m_pUIOwner = { nullptr };
    CTransform*         m_pTransform = { nullptr };

};

NS_END


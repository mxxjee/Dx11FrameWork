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
        bool        m_bRegister = false;    //register_anim을통해 호출하면 true가된다.

        bool        m_bPlay = false;

        float       m_fSpeed = 0.f;
        bool        m_bEnd = false;

        _float4     fStart = { };     //시작값, 월드좌표기준이므로 스크린->월드좌표 사용하기
        _float4     fTarget = {};    //목표값

        bool        bLoop=false;      //반복여부

    };

    typedef struct tagUICompDesc : CComponent::tagComponentDesc
    {
        UIAnimInfo          _AnimInfo[ENUM_TO_UINT(UIAnimType::END)];

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
    void            Register_Anim(UIAnimType eType, const UIAnimInfo& Info);

    void            Update_UIAnim(UIAnimType eType, _float fTimeDelta);
    void            LerpAlpha(_float vTarget, _float fLerpSpeed, _float fTimeDelta);

    void            Set_OwnerTransform(class CTransform* ptransform);
public:
            //새로운 애니메이션을 만들어서 바로 재생시킨다.
    void        PlayAnim(UIAnimType _AnimType, _float4 vStart, _float4 vTarget, float fSpeed, bool bLoop);
                
                //이미존재하는 애니메이션을 재생시킨다.
    void        PlayAnim(UIAnimType _AnimType);


private:
    void        Apply_StartValue(UIAnimType eType, UIAnimInfo& Info);
    void        Check_LoopAnimation(UIAnimType eType,   UIAnimInfo& Info,_float4 vCurrent,_float Epslion = 0.01f);
    void        Set_AnimFlag(UIAnimType eType);
public:
    static CUIComponent* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    virtual CComponent* Clone(void* pArg);
    virtual void Free() override;

private:
    CUI*                m_pUIOwner = { nullptr };
    CTransform*         m_pTransform = { nullptr };
    


private:
    float               m_ViewPortWidth;
    float               m_ViewPortHeight;

private://오너의 애니메이션타입
    UIAnimFlag                m_UIAnimType = UIAnimFlag::UI_ANIM_NONE;

    array<UIAnimInfo, ENUM_TO_UINT(UIAnimType::END)> m_AnimInfo;  //전체 애님배열

};

NS_END


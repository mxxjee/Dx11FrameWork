#pragma once
#include "CComponent.h"
NS_BEGIN(Engine)
class ENGINE_DLL CFontComponent :
    public CComponent
{

public:
    struct Font_Desc : public CComponent::COMPONENT_DESC
    {
        wstring FontName;
        _float4 vDefaultFontColor = _float4(0.f, 0.f, 0.f, 1.f);//기본색 검은색
        _float fScale = 10.f;
        _float fRotation = 0.f;
        _float2 vPosition = _float2(0.f, 0.f);
    
    };

private:
    CFontComponent(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CFontComponent(const CFontComponent& Prototype);
    virtual ~CFontComponent() = default;


public:
    virtual         HRESULT     Initialize_Prototype() override;
    virtual         HRESULT     Initialize_Copytype(void* pArg) override;


public:
    void        Set_Text(wstring _Text);

public:
    virtual HRESULT    Render();        

public:
    void        Update(XMMATRIX    WorldMatrix,_float alpha, _float fTimeDelta);
public:
    static CFontComponent* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    virtual CComponent* Clone(void* pArg);
    virtual void Free() override;

public:
    void    Set_TypingEffect(bool b) { m_bTypingEffect = b; }
    void    Set_TypingTime(_float f) { m_fTime = f; }
private:
    HRESULT Create_BS_AlphaBlend();

private:
    wstring     m_pText;
    _float2     m_vPosition;
    _float4     m_vColor;
    _float      m_vScale;
    _float      m_fRotation = 0;

    _vector     m_vSize;
    _float2     m_Origin;

   
private:
    SpriteBatch* m_pBatch = { nullptr };
    SpriteFont* m_pFont = { nullptr };

    ComPtr<ID3D11BlendState> BS_AlphaBlend;

public:
                //타이핑 효과를 재생시킨다 딱1회의 스위치 개념,내부적으로 알아서 재귀처리가 됨(문장끝을만나면 멈춤)
    void            PlayTyping();

private:
    bool        m_bTypingEffect = false;        //타이핑 하는거처럼 나오는 이펙트
    bool        m_bPlay = false;

    wstring     m_TypingStr = L"";

    float       m_fAccTime = 0.f;
    float       m_fTime = 0.5f; //몇초마다 출력?

    _uint       m_iTargetIdx = 0;//추가할 문자인덱스
    _uint       m_iTotalIndx = 0;

public:
    void            Set_StartFunction(function<void()> Func) { m_StartFunction = Func; }
    void            Set_EndFunction(function<void()> Func) { m_EndFunction = Func; }

private:
                //SetText 할때 바로 실행
    function<void()>        m_StartFunction = nullptr;

                //타이핑효과끝난 이후에 실행
    function<void()>        m_EndFunction = nullptr;


};
NS_END


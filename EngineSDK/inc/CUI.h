#pragma once
#include "CQuad.h"
#include "CUIComponent.h"

NS_BEGIN(Engine)
class ENGINE_DLL CUI :
    public CQuad
{
public:
    struct UIInfo
    {
        _float fX = {};
        _float fY = {};
        _float Depth = { 0.5f };

        _float fSizeX = {};
        _float fSizeY = {};
        _uint           iIdx = 0;
    };


public:
    typedef struct tagUIDesc : public CQuad::QUAD_DESC
    {
        _float fX = {}, fY = {},Depth = { 0.5f };
        _float fSizeX = {}, fSizeY = {};
        _uint           iIdx = 0;
        
        void* UICompDesc = nullptr;


    }UI_DESC;

protected:
    CUI(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CUI(const CUI& rhs);
    virtual ~CUI() = default;

public:
    virtual HRESULT     Initialize_Prototype(); /*원형 객체가 생성될때 부르는 Initialize*/
    virtual HRESULT     Initialize_Copytype(void* pArg); /*사본 객체가 생성될때 부르는 Initialize*/

    HRESULT             Ready_Components(void* pArg);
    HRESULT             Ready_Resource(void* pArg);

    virtual void        Update_Priority(_float fTimeDelta);
    virtual void        Update(_float fTimeDelta);
    virtual void        Update_Late(_float fTimeDelta);
    virtual void        Update_Render(_float fTimeDelta) override;

    virtual HRESULT Render();

public:
    _float          Get_Depth();
    _uint           Get_Idx() { return m_iIdx; }
    _float4         Get_ARGB() { return m_vARGB; }
    CUIComponent*   Get_UIComp() { return m_pUICom; }

public:
    void            Set_ActiveAnim(_uint Flag, function<void()> Func) { m_ActiveAnim[Flag] = Func; }
public:
    //UIEvent관련
    //활성,비활성화 상태에 따른 호출`
    virtual void        OnActivated(bool isActive);
    void                Set_Pos(_float fX, _float fY,_float fDepth);
public:
    virtual CGameObject* Clone(void* pArg)=0;
    virtual void    Free() override;

public:
    const UIInfo* Get_UIInitInfo() { return &m_UIInitInfo; }


   
private:
    function<void()>        m_ActiveAnim[2];


protected:
    _uint                     m_iIdx = 0;         //UIGroup제어용, 기본값 
    CUIComponent*             m_pUICom = nullptr;
    UIInfo                     m_UIInitInfo;
};
NS_END


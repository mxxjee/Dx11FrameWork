#pragma once
#include "CQuad.h"

NS_BEGIN(Engine)
class ENGINE_DLL CUI :
    public CQuad
{
public:
    typedef struct tagUIDesc : public CQuad::QUAD_DESC
    {
        _float fX = {}, fY = {},Depth = { 0.5f };
        _float fSizeX = {}, fSizeY = {};
        
    }UI_DESC;

protected:
    CUI(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CUI(const CUI& rhs);
    virtual ~CUI() = default;

public:
    virtual HRESULT     Initialize_Prototype(); /*원형 객체가 생성될때 부르는 Initialize*/
    virtual HRESULT     Initialize_Copytype(void* pArg); /*사본 객체가 생성될때 부르는 Initialize*/

    virtual void        Update_Priority(_float fTimeDelta);
    virtual void        Update(_float fTimeDelta);
    virtual void        Update_Late(_float fTimeDelta);
    virtual void        Update_Render(_float fTimeDelta) override;

    virtual HRESULT Render();

public:
    _float        Get_Depth();

public:
    virtual CGameObject* Clone(void* pArg)=0;
    virtual void    Free() override;


};
NS_END


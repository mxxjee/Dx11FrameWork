#pragma once
#include "CGameObject.h"

NS_BEGIN(Client)
class CScreenFilter :
    public CGameObject
{
protected:
    CScreenFilter(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CScreenFilter(const CScreenFilter& rhs);
    virtual ~CScreenFilter() = default;

public:
    virtual HRESULT     Initialize_Prototype(); /*원형 객체가 생성될때 부르는 Initialize*/
    virtual HRESULT     Initialize_Copytype(void* pArg); /*사본 객체가 생성될때 부르는 Initialize*/

    virtual void        Update_Priority(_float fTimeDelta);
    virtual void        Update(_float fTimeDelta);
    virtual void        Update_Late(_float fTimeDelta);
    virtual void        Update_Render(_float fTimeDelta);


    virtual HRESULT Render();

public:
    virtual     HRESULT     Ready_Components(void* pArg);

public:
    virtual CGameObject* Clone(void* pArg) = 0;
    virtual void    Free() override;


};

NS_END


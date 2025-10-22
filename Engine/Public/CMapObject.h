#pragma once

#include "Engine_Define.h"
#include "CGameObject.h"



NS_BEGIN(Engine)
class ENGINE_DLL CMapObject :
    public CGameObject
{

protected:
    CMapObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CMapObject(const CMapObject& rhs);
    virtual ~CMapObject() = default;


public:
    HRESULT     Initialize_Prototype(); /*원형 객체가 생성될때 부르는 Initialize*/
    virtual HRESULT     Initialize_Copytype(void* pArg); /*사본 객체가 생성될때 부르는 Initialize*/

    void        Update_Priority(_float fTimeDelta);
    void        Update(_float fTimeDelta);
    void        Update_Late(_float fTimeDelta);
    void        Update_Render(_float fTimeDelta);


    virtual HRESULT Render();


public:
    void        Set_Select(bool _bSelect) { m_bSelected = _bSelect; }
    virtual     void    Free() override;
public:
    bool            m_bSelected = false;        //(픽킹)선택여부
    _uint                   m_eRenderGroup = 0;
};

NS_END


#pragma once
#include "CGameObject.h"

namespace Engine
{
    class CInput_Manager;
}


NS_BEGIN(Client)
class CInventorySlot;
class CUI_Cursor;


class CUI_Window_Inventory :
    public CGameObject
{
protected:
    CUI_Window_Inventory(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CUI_Window_Inventory(const CUI_Window_Inventory& rhs);
    virtual ~CUI_Window_Inventory() = default;

public:
    virtual HRESULT     Initialize_Prototype(); /*원형 객체가 생성될때 부르는 Initialize*/
    virtual HRESULT     Initialize_Copytype(void* pArg); /*사본 객체가 생성될때 부르는 Initialize*/

    virtual void        Update_Priority(_float fTimeDelta);
    virtual void        Update(_float fTimeDelta);
    virtual void        Update_Late(_float fTimeDelta);
    virtual void        Update_Render(_float fTimeDelta);


    virtual HRESULT Render();


public:
    static CUI_Window_Inventory* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    virtual void    Free() override;


    // CGameObject을(를) 통해 상속됨
    virtual CGameObject* Clone(void* pArg) override;
    void        Update_Cursor();


public:
    void        Add_InvenSlots(CGameObject* pObj, int iIdx);
    void        Set_Cursor(CGameObject* pObj);

private:
    vector<CInventorySlot*>       m_InvenSlots;
    CUI_Cursor*                 m_Cursor;
    CInput_Manager*             m_pInput_Manager = nullptr;


private:
    int             m_iCurIdx = 0;
    int             maxIdx = 15;

};
NS_END


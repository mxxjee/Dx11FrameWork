#pragma once
#include "CSlot.h"
#include "CInventory_Manager.h"


NS_BEGIN(Client)

class CInventorySlot :
    public CSlot
{
private:
	CInventorySlot(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext);
	CInventorySlot(const CInventorySlot& rhs);
	virtual ~CInventorySlot() = default;

public:
	virtual HRESULT Initialize_Prototype() override; /* 원형객체가 만들어질 때 호출되는 함수. 무거운 초기화작업.  */
	virtual HRESULT Initialize_Copytype(void* pArg) override; /* 원형을 복제하여 만든 사본객체가 생성시에 호출되는 함수. 추가적인 초기화를 위해 */

	virtual void Update_Priority(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Update_Late(_float fTimeDelta) override;
	virtual void Update_Render(_float fTimeDelta) override;

	virtual HRESULT Render() override;


public:
	HRESULT			Ready_PartObject();
	void			Update_Item(ItemType eType);

public:
	static CInventorySlot* Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
	ItemType			Get_ItemType() { return m_eType; }
public:
	virtual void		Enter_State();
	virtual void		Update_State();
	bool				Is_HasItem() {return m_eType != ItemType::END;}
	void				Set_QuickSlot(bool b);
private:

	ItemType			m_eType=ItemType::END;
	int					iCount = 0; 

	bool			m_bQuickSlot = false;
	CPanel*			m_pItemPanel = nullptr;

	
};
NS_END


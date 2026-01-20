#pragma once

#include "CPanel.h"
NS_BEGIN(Client)
class CSlot
	:public CPanel
{
public:
	enum State
	{
		IDLE,HOVER,SELECT,END
	};
protected:
	CSlot(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext);
	CSlot(const CSlot& rhs);
	virtual ~CSlot() = default;

public:
	virtual HRESULT Initialize_Prototype() override; /* 원형객체가 만들어질 때 호출되는 함수. 무거운 초기화작업.  */
	virtual HRESULT Initialize_Copytype(void* pArg) override; /* 원형을 복제하여 만든 사본객체가 생성시에 호출되는 함수. 추가적인 초기화를 위해 */

	virtual void Update_Priority(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Update_Late(_float fTimeDelta) override;
	virtual void Update_Render(_float fTimeDelta) override;

	virtual HRESULT Render() override;


public:
	virtual void		Enter_State();
	virtual void		Update_State();

public:
	void		Change_State(State iState);

	

protected:
	State		m_ePreState= State::END;
	State		m_eState=State::IDLE;

	vector<wstring>		m_TexKeys;


};

NS_END

#pragma once
#include "CUI.h"

NS_BEGIN(Client)
class CBackGround :
    public CUI
{
private:
    CBackGround(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CBackGround(const CBackGround& rhs);
    virtual ~CBackGround() = default;

public:
	  virtual HRESULT Initialize_Prototype() override; /* 원형객체가 만들어질 때 호출되는 함수. 무거운 초기화작업.  */
	  virtual HRESULT Initialize_Copytype(void* pArg) override; /* 원형을 복제하여 만든 사본객체가 생성시에 호출되는 함수. 추가적인 초기화를 위해 */
	  virtual void Update_Priority(_float fTimeDelta) override;
	  virtual void Update(_float fTimeDelta) override;
	  virtual void Update_Late(_float fTimeDelta) override;
	  virtual HRESULT Render() override;


public:
	static CBackGround* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};
NS_END

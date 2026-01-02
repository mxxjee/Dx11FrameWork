#pragma once
#include "CPanel.h"

NS_BEGIN(Client)
class CFadeScreen :
    public CPanel
{
public:
	enum State
	{
		WAIT,START_FADEIN,LOOP_FADE_IN,END_FADEIN,START_FADEOUT,LOOP_FADEOUT,END_FADEOUT,END
	};
private:
	CFadeScreen(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext);
	CFadeScreen(const CFadeScreen& rhs);
    virtual ~CFadeScreen() = default;


public:
    void        PlayFadeIn();
    void        PlayFadeOut();

	void		Set_AutoMode(bool b) { m_bAutoMode = b; }
	void		Set_AutoTime(_float fTime) { m_fTime = fTime; }
public:
	virtual HRESULT Initialize_Prototype() override; /* 원형객체가 만들어질 때 호출되는 함수. 무거운 초기화작업.  */
	virtual HRESULT Initialize_Copytype(void* pArg) override; /* 원형을 복제하여 만든 사본객체가 생성시에 호출되는 함수. 추가적인 초기화를 위해 */

	virtual void Update_Priority(_float fTimeDelta) override;
	virtual void Update(_float fTimeDelta) override;
	virtual void Update_Late(_float fTimeDelta) override;
	virtual void Update_Render(_float fTimeDelta) override;

	virtual HRESULT Render() override;


private:
	void			IfFadeOutEnd();
	void			IfFadeOutStart();

	void			IfFadeInEnd();
	void			IfFadeInStart();

private:
	void			Change_State();
public:
	static CFadeScreen* Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

public:
	void			Set_FadeInEndFunc(std::function<void()> Func) { m_FadeInEndFunction = Func; }
private:
	bool			m_bAutoMode = false;			//오토모드 true이면 특정 시간이후에 다시 알아서 fadeout실행

	float			m_fCurTime = 0.f;
	float			m_fTime = 0.f;
	bool			m_bEnd = false;


private:
	State				m_eState = WAIT;
	State				m_ePreState = END;

private:
	std::function<void()>		m_FadeInEndFunction = nullptr;

};
NS_END


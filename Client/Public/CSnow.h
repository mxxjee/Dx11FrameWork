#pragma once
#include "CGameObject.h"

NS_BEGIN(Engine)
class CVIBuffer_Particle_Rect;
class CShader;
class CTexture;
NS_END


NS_BEGIN(Client)
class CSnow :
    public CGameObject
{
protected:
	CSnow(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CSnow(const CSnow& Prototype); /* 메모리 복사를 통해 객체를 생성. */
	virtual ~CSnow() = default;

public:
	virtual HRESULT Initialize_Prototype(); /* 원형객체가 만들어질 때 호출되는 함수. 무거운 초기화작업.  */
	virtual HRESULT Initialize_Copytype(void* pArg); /* 원형을 복제하여 만든 사본객체가 생성시에 호출되는 함수. 추가적인 초기화를 위해 */
	virtual void Update_Priority(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Update_Late(_float fTimeDelta);
	virtual void        Update_Render(_float fTimeDelta) override;

	virtual HRESULT Render();


private:
	CVIBuffer_Particle_Rect* m_pVIBuffer = { nullptr };
	CTexture* m_pTexture = { nullptr };
	CShader* m_pShader = { nullptr };

private:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

public:
	static CSnow* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END
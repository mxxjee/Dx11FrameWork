#pragma once
#include "CUI.h"
#include "VertexData.h"

NS_BEGIN(Client)

class CBackGround :
    public CUI
{
private:
    CBackGround(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext);
    CBackGround(const CBackGround& rhs);
    virtual ~CBackGround() = default;

public:
	  virtual HRESULT Initialize_Prototype() override; /* 원형객체가 만들어질 때 호출되는 함수. 무거운 초기화작업.  */
	  virtual HRESULT Initialize_Copytype(void* pArg) override; /* 원형을 복제하여 만든 사본객체가 생성시에 호출되는 함수. 추가적인 초기화를 위해 */
	 
	  virtual void Update_Priority(_float fTimeDelta) override;
	  virtual void Update(_float fTimeDelta) override;
	  virtual void Update_Late(_float fTimeDelta) override;
	  virtual void Update_Render(_float fTimeDelta) override;

	  virtual HRESULT Render() override;


private:
	HRESULT		Initialize_Piepline();
	HRESULT		Add_Component(void *pArg);

private:
	void	CreateGeometry();
	void	CreateInputLayout();
	void	VertexShader();
	void	PixelShader();

	HRESULT		CreateRasterizerState();
	HRESULT		CreateSamplerState();
	HRESULT		CreateBlendState();

private:
	void		Set_IA();
	void		Set_VS();
	void		Set_RS();
	void		Set_PS();
	void		Set_OM();


public:
	static CBackGround* Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;


private:
	RenderPipelineResource<VertexTextureData,	TransformData> m_Pipeline;
	TransformData m_transformData;

private:
	_float3	m_vLocalScale = { 1.f,1.f,0.f };
	_float3	m_vLocalRotate = { 0.f,0.f,0.f };
	_float3	m_vLocalPosition = { 0.f,0.f,0.f };


};
NS_END

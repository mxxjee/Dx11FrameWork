#pragma once
#include "CVIBuffer_Particle.h"

NS_BEGIN(Engine)
class ENGINE_DLL CVIBuffer_Particle_Rect :
    public CVIBuffer_Particle
{
public:
	typedef struct tagVIBuffer_Particle_Rect_Desc : public CVIBuffer_Particle::PARTICLE_DESC
	{
		//나중에 필요하면 추가

	}PARTICLE_RECT_DESC;

private:
	CVIBuffer_Particle_Rect(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CVIBuffer_Particle_Rect(const CVIBuffer_Particle_Rect& Prototype);
	virtual ~CVIBuffer_Particle_Rect() = default;


public:
	virtual HRESULT Initialize_Prototype(const PARTICLE_DESC* pInitialDesc);
	virtual HRESULT Initialize_Copytype(void* pArg) override;

public:
	static CVIBuffer_Particle_Rect* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const PARTICLE_DESC* pInitialDesc);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END


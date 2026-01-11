#pragma once
#include "CVIBuffer_Particle.h"


NS_BEGIN(Engine)
class ENGINE_DLL CVIBuffer_Particle_Point :
    public CVIBuffer_Particle
{
public:
	typedef struct tagVIBuffer_Particle_Rect_Desc : public CVIBuffer_Particle::PARTICLE_DESC
	{
		//나중에 필요하면 추가

	}PARTICLE_RECT_DESC;

private:
	CVIBuffer_Particle_Point(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	CVIBuffer_Particle_Point(const CVIBuffer_Particle_Point& Prototype);
	virtual ~CVIBuffer_Particle_Point() = default;


public:
	HRESULT Initialize_Prototype();
	virtual HRESULT Initialize_Copytype(void* pArg) override;

			//버퍼 재생성
	HRESULT			Reset_Particle(const tagVIBuffer_Particle_Desc& pData);

		//매프레임 데이터갱신
	void		Update_Buffer(const vector<tagVertexInstance>& InstanceData);


public:
	//인스턴싱의 렌더방식과 바인드 방식이 다르므로 오버라이드
	virtual         HRESULT     Bind_Resource();
	virtual         HRESULT     Render();
public:
	static CVIBuffer_Particle_Point* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

private:
	int			m_iCurrentNumInstance = 0;

};

NS_END


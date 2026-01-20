#pragma once
#include "CBase.h"
#include "ConstantStruct.h"
/*카메라의 뷰 투영을 보관해주는기능
쉐이더에 바인딩 하는기능.
뷰/투영의 역행렬을 보관 -> (셰이더에서 쓰임)
카메라 위치를 구해놓는다.*/

NS_BEGIN(Engine)
class CPipeLine final :
    public CBase
{
private:
	struct PIPE_DATA
	{
		_float4x4		m_TransformMatrices[ENUM_TO_UINT(D3DTS::END)] = {};
		_float4x4		m_TransformInverseMatrices[ENUM_TO_UINT(D3DTS::END)] = {};
		_float4				m_vCamPosition;
	};


private:
	CPipeLine(ComPtr<ID3D11Device>	_pDevice, ComPtr<ID3D11DeviceContext>	_pContext);
	virtual ~CPipeLine() = default;



public:
	void		Set_Transform(_uint CameraType, D3DTS eTransformMatrix, _fmatrix TransformMatrix);
	void		Set_CamPosition(_uint CameraType,const _float4& Position);
public:
	//뷰,투영을 골라서 바인드가능
	HRESULT Bind_PipeLineMatrix(class CShader* pShader, const _char* pConstant, _uint iCameraType,D3DTS eTransformMatrix);
	
	//뷰와 투영을 한번에 바인드
	HRESULT Bind_PipeLineMatrixAll(class CShader* pShader, const _char* pConstant, _uint iCameraType);

	HRESULT Bind_PipeLineInverseMatrix(class CShader* pShader, const _char* pConstant, _uint iCameraType,D3DTS eTransformMatrix);
	
	//역행렬 구해오기
	const _float4x4* Get_InverseTransform(_uint iCameraType, D3DTS eTransformMatrix)
	{
		return &m_PipeDatas[ENUM_TO_UINT(iCameraType)].m_TransformInverseMatrices[(ENUM_TO_UINT(eTransformMatrix))];
	}
public:
	/*상수버퍼에 던질 구조체 업데이트*/
	HRESULT Update_CamBuffer(_uint CameraType);
	const CameraBuffer& Get_CameraBuffer() { return m_CameraBuffer; }

public:
	//매프레임마다 역행렬을 구해서 저장한다.
	void	Update();	

public:
	const _float4x4&		Get_ViewMatrix(_uint CameraType);
	const _float4x4&		Get_ProjMatrix(_uint CameraType);
	_matrix					Get_ViewProjMatrix(_uint CameraType);

	const _float4&			Get_CamPosition(_uint CameraType);

private:
	vector<PIPE_DATA>		m_PipeDatas;


public:
	static CPipeLine* Create(ComPtr<ID3D11Device>	_pDevice, ComPtr<ID3D11DeviceContext>	_pContext);
	virtual void Free() override;


private:
	ComPtr<ID3D11Device>			m_pDevice;
	ComPtr<ID3D11DeviceContext>		m_pContext;


private:
	CameraBuffer				m_CameraBuffer;
};


NS_END


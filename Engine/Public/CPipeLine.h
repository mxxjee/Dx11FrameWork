#pragma once
#include "CBase.h"
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
	CPipeLine();
	virtual ~CPipeLine() = default;



public:
	void		Set_Transform(_uint CameraType, D3DTS eTransformMatrix, _fmatrix TransformMatrix);
public:
	HRESULT Bind_PipeLineMatrix(class CShader* pShader, const _char* pConstant, _uint iCameraType,D3DTS eTransformMatrix);
	HRESULT Bind_PipeLineInverseMatrix(class CShader* pShader, const _char* pConstant, _uint iCameraType,D3DTS eTransformMatrix);
	HRESULT Bind_CamPosition(class CShader* pShader, const _char* pConstant ,_uint iCameraType );

	//매프레임마다 역행렬을 구해서 저장한다.
	void	Update();	

private:
	vector<PIPE_DATA>		m_PipeDatas;


public:
	static CPipeLine* Create();
	virtual void Free() override;
};


NS_END


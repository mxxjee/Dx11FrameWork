#include "CPipeLine.h"
#include "CShader.h"


CPipeLine::CPipeLine()
{
}

void CPipeLine::Set_Transform(_uint CameraType, D3DTS eTransformMatrix, _fmatrix TransformMatrix)
{
	XMStoreFloat4x4(&m_PipeDatas[ENUM_TO_UINT(CameraType)].m_TransformMatrices[ENUM_TO_UINT(eTransformMatrix)], TransformMatrix);

}

HRESULT CPipeLine::Bind_PipeLineMatrix(CShader* pShader, const _char* pConstant, _uint iCameraType, D3DTS eTransformMatrix)
{
	return pShader->Bind_Matrix(pConstant,m_PipeDatas[ENUM_TO_UINT(iCameraType)].m_TransformMatrices[ENUM_TO_UINT(eTransformMatrix)]);

}

HRESULT CPipeLine::Bind_PipeLineMatrixAll(CShader* pShader, const _char* pConstant, _uint iCameraType)
{

	_matrix ViewProj = XMMatrixMultiply(XMLoadFloat4x4(&m_PipeDatas[ENUM_TO_UINT(iCameraType)].m_TransformMatrices[ENUM_TO_UINT(D3DTS::VIEW)]),
		XMLoadFloat4x4(&m_PipeDatas[ENUM_TO_UINT(iCameraType)].m_TransformMatrices[ENUM_TO_UINT(D3DTS::PROJ)]));

	_float4x4	fViewProj;
	XMStoreFloat4x4(&fViewProj, ViewProj);

	return pShader->Bind_Matrix(pConstant, fViewProj);
}

HRESULT CPipeLine::Bind_PipeLineInverseMatrix(CShader* pShader, const _char* pConstant, _uint iCameraType, D3DTS eTransformMatrix)
{
	return pShader->Bind_Matrix(pConstant, m_PipeDatas[ENUM_TO_UINT(iCameraType)].m_TransformInverseMatrices[ENUM_TO_UINT(eTransformMatrix)]);

}

HRESULT CPipeLine::Bind_CamPosition(CShader* pShader, const _char* pConstant, _uint iCameraType)
{
	return pShader->Bind_RawValue(pConstant, &m_PipeDatas[ENUM_TO_UINT(iCameraType)].m_vCamPosition,sizeof(_float4));
}

void CPipeLine::Update()
{
	for (int i = 0; i < m_PipeDatas.size(); ++i)
	{
		PIPE_DATA& Data = m_PipeDatas[i];
		for (int j = 0; j < ENUM_TO_UINT(D3DTS::END); ++j)
		{
			XMStoreFloat4x4(&Data.m_TransformInverseMatrices[j], XMMatrixInverse(nullptr, XMLoadFloat4x4(&Data.m_TransformMatrices[j])));

		}

		memcpy(&Data.m_vCamPosition, &Data.m_TransformMatrices[ENUM_TO_UINT(D3DTS::VIEW)].m[3], sizeof(_float4));

	}
}

const _float4x4& CPipeLine::Get_ViewMatrix(_uint CameraType)
{
	return m_PipeDatas[CameraType].m_TransformMatrices[ENUM_TO_UINT(D3DTS::VIEW)];
}

const _float4x4& CPipeLine::Get_ProjMatrix(_uint CameraType)
{
	return m_PipeDatas[CameraType].m_TransformMatrices[ENUM_TO_UINT(D3DTS::PROJ)];
}

const _float4& CPipeLine::Get_CamPosition(_uint CameraType)
{
	return m_PipeDatas[CameraType].m_vCamPosition;
}

CPipeLine* CPipeLine::Create()
{
	CPipeLine* pInstance = new CPipeLine();
	pInstance->m_PipeDatas.resize(ENUM_TO_UINT(CAMERA_TYPE::END));

	return pInstance;
}

void CPipeLine::Free()
{
	__super::Free();
}

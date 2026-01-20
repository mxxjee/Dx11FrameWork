#include "CNavigation.h"

CNavigation::CNavigation(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CComponent(pDevice, pContext)

{
}

CNavigation::CNavigation(const CNavigation& Prototype)
	: CComponent(Prototype)
{
}

HRESULT CNavigation::Initialize_Prototype(const _tchar* pNavigationData)
{
	//파일읽어서 셀생성해주시요
	_ulong dwByte = {};
	HANDLE   hFile=CreateFile(pNavigationData, GENERIC_READ,0,nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == 0)
		return E_FAIL;

	//정점3개씩 읽으세요
	_float3		vPoints[ENUM_TO_UINT(POINT::POINT_END)];
	while (true)
	{
		bool b= ReadFile(hFile, vPoints, sizeof(_float3) * ENUM_TO_UINT(POINT::POINT_END), &dwByte, nullptr);
		if (0 == dwByte)
			break;

		//셀 생성


	
	
	}
	return S_OK;
}

HRESULT CNavigation::Initialize_Copytype(void* pArg)
{
	return S_OK;
}

HRESULT CNavigation::SetUp_Neighbors()
{
	return S_OK;
}

CNavigation* CNavigation::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pNavigationData)
{
	return nullptr;
}

CComponent* CNavigation::Clone(void* pArg)
{
	return nullptr;
}

void CNavigation::Free()
{
}

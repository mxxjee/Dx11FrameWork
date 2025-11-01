#include "CShader_Manager.h"
#include "CShader.h"
#include "CGameInstance.h"
#include "CConstantBuffer.h"


CShader_Manager::CShader_Manager(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pContext)
	:m_pDevice{ _pDevice }, m_pContext{ _pContext }
{
}

HRESULT CShader_Manager::Initialize()
{
	if (FAILED(Create_GlobalConstantBuffer()))
		return E_FAIL;

    return S_OK;
}



HRESULT CShader_Manager::Register_Shader(const _wstring& Tag, CShader* pInstance)
{
	CShader* pShader = Find_Shader(Tag);
	if (pShader)
	{
		MSG_BOX("Shader is alreday exist");
		return E_FAIL;
	}

	else
	{
		m_mapShader.emplace(Tag, pInstance);

		if (FAILED(Set_ConstantBuffer(pInstance)))
			return E_FAIL;

	}
	

	return S_OK;
}

CShader_Manager* CShader_Manager::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pContext)
{
	CShader_Manager* pInstance = new CShader_Manager(_pDevice, _pContext);
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Create : CShader_Manager");
		Safe_Release(pInstance);
	}

	return pInstance;

}

void CShader_Manager::Free()
{
	for (auto& pair : m_mapShader)
	{
		Safe_Release(pair.second);

	}

	for (auto& pair : m_ConstantBuffers)
	{
		if (pair.second)
			Safe_Release(pair.second);
	}

}



HRESULT CShader_Manager::Set_ConstantBuffer(CShader* pInstance)
{

	/*초기세팅*/
	for (auto& pair : m_ConstantBuffers)
	{
		if (FAILED(pInstance->Set_ConstantBuffer(pair.first, pair.second->GetComPtr().Get())))
			return E_FAIL;

	}

	return S_OK;
	/*if(FAILED(pInstance->Set_ConstantBuffer("CameraBuffer", m_pCameraBuffer->GetComPtr().Get())))
		return E_FAIL;

	if (pInstance->Set_ConstantBuffer("DirectionLightBuffer", m_pDirectionLightBuffer->GetComPtr().Get()))
		return E_FAIL;


	if (pInstance->Set_ConstantBuffer("PointLightBuffer", m_pPointLightBuffer->GetComPtr().Get()))
		return E_FAIL;*/


}

HRESULT CShader_Manager::Bind_GlobalLightData()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	for (auto& pair : m_mapShader)
	{
		if (pair.second)
		{
			pGameInstance->Bind_Lights(pair.second);
		}
	}
	return S_OK;
}

HRESULT CShader_Manager::Bind_SamplerState(_uint iRenderGroup)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	for (auto& pair : m_mapShader)
	{
		if (pair.second)
		{
			pGameInstance->Bind_SamplerState(pair.second,iRenderGroup);
		}
	}
	return S_OK;
}

void CShader_Manager::CopyData_Buffer(string Key, const void* pData, _uint iSize)
{
	auto it = m_ConstantBuffers.find(Key);
	if (it->second)
	{
		it->second->CopyData(pData, iSize);
	}
}

CShader* CShader_Manager::Find_Shader(const _wstring& Tag)
{
	auto iter = m_mapShader.find(Tag);

	if (iter != m_mapShader.end())
		return iter->second;


    return nullptr;
}

HRESULT CShader_Manager::Create_GlobalConstantBuffer()
{

	CConstantBuffer* pCamera_Buffer = new CConstantBuffer(m_pDevice,m_pContext);
	pCamera_Buffer->Create(0,sizeof(CameraBuffer));
	m_ConstantBuffers.emplace("CameraBuffer", pCamera_Buffer);


	CConstantBuffer* pDirectionLightBuffer = new CConstantBuffer(m_pDevice, m_pContext);
	pDirectionLightBuffer->Create(1,sizeof(DirectionLightBuffer));
	m_ConstantBuffers.emplace("DirectionLightBuffer", pDirectionLightBuffer);

	CConstantBuffer* pPointLightBuffer = new CConstantBuffer(m_pDevice, m_pContext);
	pPointLightBuffer->Create(2,sizeof(PointLightBuffer));
	m_ConstantBuffers.emplace("PointLightBuffer", pPointLightBuffer);


	return S_OK;
}

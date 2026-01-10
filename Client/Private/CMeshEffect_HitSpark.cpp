#include "CMeshEffect_HitSpark.h"
#include "CEffectData_Manager.h"	
#include "CShader.h"

USING(Client)
CMeshEffect_HitSpark::CMeshEffect_HitSpark(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CMeshEffect(pDevice,pContext)
{
}

CMeshEffect_HitSpark::CMeshEffect_HitSpark(const CMeshEffect_HitSpark& rhs)
	:CMeshEffect(rhs)
{
}

HRESULT CMeshEffect_HitSpark::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;


	return S_OK;
}

HRESULT CMeshEffect_HitSpark::Initialize_Copytype(void* pArg)
{
	HITSPARK_DESC* pDesc = static_cast<HITSPARK_DESC*>(pArg);
	m_bUseParentRotation = pDesc->bUseParentRotation;

	if (FAILED(__super::Initialize_Copytype(pArg)))
		return E_FAIL;

	_float fInitRotateZ[] = { 45.f,135.f,225.f,315.f };
	//_float4 fInitPosition[] = { _float4(0.f,2.f,0.f,1.f),
	// _float4(0.f,2.f,0.f,1.f),
	//_float4(0.f,2.f,0.f,1.f),
	//_float4(0.f,2.f,0.f,1.f),
	//};

	m_InitSpeed = 3.f;


	for (int i = 0; i < 4; ++i)
	{
		m_InitRotationZ[i] = fInitRotateZ[i];

		m_InitPosition[i]= m_LocalData.InitOffSet;
		m_CurrentPos[i] = m_InitPosition[i];
	}

	return S_OK;
}

void CMeshEffect_HitSpark::Update_Priority(_float fTimeDelta)
{
	__super::Update_Priority(fTimeDelta);
}

void CMeshEffect_HitSpark::Update(_float fTimeDelta)
{
	CheckTrue(m_bStop);
	__super::Update(fTimeDelta);
	for (int i = 0; i < 4; ++i)
	{
		// 행렬을 통해 방향을 추출하는 것이 가장 안전함
		_float fYaw = XMConvertToRadians(m_InitRotationZ[i]);
		_matrix matRot = XMMatrixRotationZ(fYaw);

		_vector vDir = matRot.r[1]; //(Y축)으로 날아간다

		_vector vPos = XMLoadFloat4(&m_CurrentPos[i]);
		vPos += vDir * m_InitSpeed * fTimeDelta;
		XMStoreFloat4(&m_CurrentPos[i], vPos);

	}
	
}


void CMeshEffect_HitSpark::Update_Late(_float fTimeDelta)
{
	CheckTrue(m_bStop);
	__super::Update_Late(fTimeDelta);
	Make_PartWorldMatrix();
	

}

void CMeshEffect_HitSpark::Update_Render(_float fTimeDelta)
{
	__super::Update_Render(fTimeDelta);
}

HRESULT CMeshEffect_HitSpark::Render()
{
	//공통적인거 먼저바인딩
	__super::Bind_ShaderResources();

	for (int i = 0; i < 4; ++i)
	{
		iTargetIdx = i;
		__super::Render();
	}
	return S_OK;
}

CMeshEffect_HitSpark* CMeshEffect_HitSpark::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
{
	CMeshEffect_HitSpark* pInstance = new CMeshEffect_HitSpark(_pDevice, _pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CMeshEffect_HitSpark");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CMeshEffect_HitSpark::Clone(void* pArg)
{
	CMeshEffect_HitSpark* pInstance = new CMeshEffect_HitSpark(*this);

	if (FAILED(pInstance->Initialize_Copytype(pArg)))
	{
		MSG_BOX("Failed to Created : CMeshEffect_HitSpark");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMeshEffect_HitSpark::Free()
{
	__super::Free();
}

HRESULT CMeshEffect_HitSpark::Bind_ShaderResources()
{

	_float4x4 PartWorldMatrix;
	XMStoreFloat4x4(&PartWorldMatrix, m_PartsWorldMatrix[iTargetIdx]);

	if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", PartWorldMatrix)))
		return E_FAIL;

	
	if (FAILED(m_pShader->Bind_Float("g_Alpha", m_fAlpha)))
		return E_FAIL;

	return S_OK;
}

void CMeshEffect_HitSpark::Play()
{
	__super::Play();
	m_LocalData.InitRotation.y = 0.f;
	for (int i = 0; i < 4;++i)
		m_CurrentPos[i] = m_LocalData.InitOffSet;


}

void CMeshEffect_HitSpark::Stop()
{
	__super::Stop();
	m_LocalData.InitRotation.y = 0.f;

}

void CMeshEffect_HitSpark::Make_PartWorldMatrix()
{
	_matrix Scaling = XMMatrixScaling(m_InitScale.x, m_InitScale.y, m_InitScale.z);
	_matrix mat = XMLoadFloat4x4(&OriginMatrix);

	if(!m_bUseParentRotation)
	{
		_vector vWorldPos = XMLoadFloat4(&(_float4&)CombinedMatrix.m[3][0]);
		mat = XMMatrixTranslationFromVector(vWorldPos);
	}

	for (int i = 0; i < 4; ++i)
	{

		_float Yaw = XMConvertToRadians(m_InitRotationZ[i]);
		_matrix Rotation = XMMatrixRotationRollPitchYaw(0.f, 0.f, Yaw);
		_matrix Translate = XMMatrixTranslation(m_CurrentPos[i].x,
			m_CurrentPos[i].y, m_CurrentPos[i].z);


		m_LocalMatrix[i] = Scaling * Rotation * Translate;



		m_PartsWorldMatrix[i] = m_LocalMatrix[i] * mat;
	}

}

#ifdef _DEBUG
void CMeshEffect_HitSpark::Render_DebugImgui()
{
	if (ImGui::ColorEdit4("Color", (float*)&m_LocalData.vColor))
	{
		m_pEffectData_Manager->Update_Data(m_DataName, static_cast<MeshEffectData*>(&m_LocalData));
		Make_LocalMatrix();
	}

	if (ImGui::DragFloat("Itensity", (float*)&m_LocalData.fIntensity))
	{
		m_pEffectData_Manager->Update_Data(m_DataName, static_cast<MeshEffectData*>(&m_LocalData));

	}


	//파츠의 스케일지정
	if (ImGui::DragFloat4("InitOffSet", (float*)&m_LocalData.InitOffSet))
	{
		m_pEffectData_Manager->Update_Data(m_DataName, static_cast<MeshEffectData*>(&m_LocalData));
		Make_LocalMatrix();
	}


	if (ImGui::Checkbox("Loop", (bool*)&m_LocalData.m_bLoop))
	{
		m_pEffectData_Manager->Update_Data(m_DataName, static_cast<MeshEffectData*>(&m_LocalData));

	}


	if (ImGui::DragFloat("LifeTime", (float*)&m_LocalData.fLifeTime))
	{
		m_pEffectData_Manager->Update_Data(m_DataName, static_cast<MeshEffectData*>(&m_LocalData));

	}


	if (ImGui::Button("Play"))
		Play();


	if (ImGui::Button("Stop"))
		Stop();

	if (ImGui::Button("Save"))
	{
		m_pEffectData_Manager->Save_To_Json(m_DataName, static_cast<MeshEffectData*>(&m_LocalData));


	}

}

#endif // _DEBUG


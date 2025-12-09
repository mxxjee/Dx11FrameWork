#include "CAnimBody.h"
#include "CBody.h"	
#include "CAnimation.h"
#include "CModel.h"
#include "CModelObject.h"
#include "CGameInstance.h"
#include "CMeshComponent.h"
#include "CShader.h"

CAnimBody::CAnimBody(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CBody(pDevice,pContext)
{
}

CAnimBody::CAnimBody(const CAnimBody& rhs)
	:CBody(rhs)
{
}

HRESULT CAnimBody::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CAnimBody::Initialize_Copytype(void* pArg)
{
	ANIMBODY_DESC* pDesc = static_cast<ANIMBODY_DESC*>(pArg);
	m_pParentState = pDesc->pParentState;

	if (FAILED(__super::Initialize_Copytype(pArg)))
		return E_FAIL;

	m_fInitTransitionTime = 0.2f;


	return S_OK;
}

void CAnimBody::Update_Priority(_float fTimeDelta)
{
	__super::Update_Priority(fTimeDelta);

}

void CAnimBody::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);


}

void CAnimBody::Update_Late(_float fTimeDelta)
{
	if (m_pModel && m_pModel->Get_NumAnim() > 0)
		m_pModel->Play_Animation(fTimeDelta);
}

void CAnimBody::Update_Render(_float fTimeDelta)
{
	__super::Update_Render(fTimeDelta);

}

HRESULT CAnimBody::Render()
{

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;


	for (auto& Mesh : m_pModel->Get_Meshs())
	{
		/*모든 메쉬를 순회하면서 바인드한다.
		   각 메쉬들의 위치와 소유한 메테리얼의 이미지 바인딩.
		   이후 메쉬를 그리는 작업*/

		if (Mesh.second)
		{
			Mesh.second->Bind_ShaderResource(m_pShader, "g_DiffuseTexture", aiTextureType::aiTextureType_DIFFUSE);
			Mesh.second->Bind_ShaderResource(m_pShader, "g_SpecularTexture", aiTextureType::aiTextureType_SPECULAR);
			Mesh.second->Bind_ShaderResource(m_pShader, "g_AmbientTexture", aiTextureType::aiTextureType_AMBIENT);

			if (FAILED(m_pModel->Bind_Bones(m_pShader, "g_BoneMatrices", Mesh.second)))
				return E_FAIL;

			if (FAILED(m_pShader->Begin(Mesh.second->Get_PassName())))
				return E_FAIL;

			if (FAILED(m_pModel->Render(Mesh.second)))
				return E_FAIL;


		}



	}


	return S_OK;
}

void CAnimBody::Set_Animation_Speed(const wstring& AnimName, _float fSpeed)
{
	if (m_pModel)
		m_pModel->Set_Animation_Speed(AnimName, fSpeed);

}

bool CAnimBody::Get_IsAnimFinished()
{
	CheckNullResult(m_pModel, false);
	CheckTrueResult(m_pModel->Get_NumAnim() == 0, false);


	return m_pModel->Get_IsAnimFinished();
}

CAnimBody* CAnimBody::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
{
	CAnimBody* pInstance = new CAnimBody(_pDevice, _pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CAnimBody");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject* CAnimBody::Clone(void* pArg)
{
	CAnimBody* pInstance = new CAnimBody(*this);

	if (FAILED(pInstance->Initialize_Copytype(pArg)))
	{
		MSG_BOX("Failed to Cloned : CAnimBody");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CAnimBody::Reserve_Animation(_wstring AnimKey, bool bLoop, bool immediately)
{
	m_NextAnimKey = AnimKey;
	m_NextAnimLoop = bLoop;

	if (immediately)
		m_pModel->Set_TransitionTime(0.05f);

	else
		m_pModel->Set_TransitionTime(m_fInitTransitionTime);
}

wstring CAnimBody::Get_CurrentAnimKey()
{
	return m_pModel->Get_CurrentAnimKey();
}

HRESULT CAnimBody::Ready_Components(void* pArg)
{

	CheckNullResult(pArg, E_FAIL);
	ANIMBODY_DESC* pAnimBodyDesc = static_cast<ANIMBODY_DESC*>(pArg);
	if (pAnimBodyDesc)
	{
		CModel::MODEL_DSC pModelDesc;
		pModelDesc.pOwner = this;
		m_pModel = m_pGameInstance->Clone_Model(pAnimBodyDesc->modelName, &pModelDesc);

		if (!m_pModel)
			return E_FAIL;
	}

	return S_OK;
}


void CAnimBody::Free()
{
	__super::Free();
}

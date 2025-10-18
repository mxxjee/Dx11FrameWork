#include "CUICamera.h"
#include "COrthographicCameraComponent.h"
#include "CGameInstance.h"
#include "CShader.h"

CUICamera::CUICamera(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CCamera_Base(pDevice, pContext)
{
}

CUICamera::CUICamera(const CUICamera& rhs)
    : CCamera_Base(rhs)
{
}

HRESULT CUICamera::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUICamera::Initialize_Copytype(void* pArg)
{
	if (FAILED(__super::Initialize_Copytype(pArg)))
		return E_FAIL;

	m_bPerspective = false;
	Update_PipeLine();

	return S_OK;
}

void CUICamera::Update_Priority(_float fTimeDelta)
{
	__super::Update_Priority(fTimeDelta);
}

void CUICamera::Update(_float fTimeDelta)
{
	//__super::Update(fTimeDelta);


}

void CUICamera::Update_Late(_float fTimeDelta)
{
	//__super::Update_Late(fTimeDelta);

	//m_pCameraCom->Update_ViewMatrix(fTimeDelta);

}

void CUICamera::Update_Render(_float fTimeDelta)
{
	__super::Update_Render(fTimeDelta);
}

HRESULT CUICamera::Render()
{
	
    return S_OK;
}

void CUICamera::Set_Target(CGameObject* pTarget)
{
}

void CUICamera::Follow_Target(_float fTimeDelta)
{
}



CUICamera* CUICamera::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
{
	CUICamera* pInstance = new CUICamera(_pDevice, _pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create :CUICamera ");
		Safe_Release(pInstance);

	}
	return pInstance;
}

CGameObject* CUICamera::Clone(void* pArg)
{
	CUICamera* pInstance = new CUICamera(*this);
	if (FAILED(pInstance->Initialize_Copytype(pArg)))
	{
		MSG_BOX("Failed to Cloned :CUICamera ");
		Safe_Release(pInstance);

	}
	return pInstance;
}

void CUICamera::Free()
{


	__super::Free();
}

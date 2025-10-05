#include "CMainCamera.h"
#include "CPerspectiveCameraComponent.h"
#include "CGameInstance.h"


CMainCamera::CMainCamera(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CGameObject(pDevice,pContext)
{
}

CMainCamera::CMainCamera(const CMainCamera& rhs)
	:CGameObject(rhs)
{
}

HRESULT CMainCamera::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMainCamera::Initialize_Copytype(void* pArg)
{
	if(FAILED(__super::Initialize_Copytype(pArg)))
		return E_FAIL;

	GAMEOBJECT_DESC* pDesc = static_cast<GAMEOBJECT_DESC*>(pArg);

	m_pPerspectiveCameraCom= dynamic_cast<CPerspectiveCameraComponent*>(m_pGameInstance->Clone_Prototype
	(PROTOTYPE::COMPONENT, 0, PROTO_COMPONENT_NAME(L"PerspectiveCamera"), pDesc));

	CheckNullResult(m_pPerspectiveCameraCom, E_FAIL);

	Safe_AddRef(m_pPerspectiveCameraCom);
	m_Components.emplace(L"PerspectiveCamera", m_pPerspectiveCameraCom);


	return S_OK;
}

void CMainCamera::Update_Priority(_float fTimeDelta)
{
	__super::Update_Priority(fTimeDelta);
}

void CMainCamera::Update(_float fTimeDelta)
{
	__super::Update(fTimeDelta);
	if (GetKeyState(VK_RIGHT) & 0x8000)
		m_pTransformCom->Move(DIRECTION::RIGHT, fTimeDelta);


	else if (GetKeyState(VK_LEFT) & 0x8000)
		m_pTransformCom->Move(DIRECTION::LEFT, fTimeDelta);

	else if (GetKeyState(VK_UP) & 0x8000)
		m_pTransformCom->Move(DIRECTION::UP, fTimeDelta);

	else if (GetKeyState(VK_DOWN) & 0x800)
		m_pTransformCom->Move(DIRECTION::DOWN, fTimeDelta);

	else if (GetKeyState('W') & 0x800)
		m_pTransformCom->Move(DIRECTION::FORWARD, fTimeDelta);

	else if (GetKeyState('S') & 0x800)
		m_pTransformCom->Move(DIRECTION::BACKWARD, fTimeDelta);

}

void CMainCamera::Update_Late(_float fTimeDelta)
{
	__super::Update_Late(fTimeDelta);
	m_pPerspectiveCameraCom->Update_ViewMatrix(fTimeDelta);

	
}

void CMainCamera::Update_Render(_float fTimeDelta)
{
	__super::Update_Render(fTimeDelta);
}

HRESULT CMainCamera::Render()
{
	return S_OK;
}

CMainCamera* CMainCamera::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
{
	CMainCamera* pInstance = new CMainCamera(_pDevice, _pDeviceContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create :CMainCamera ");
		Safe_Release(pInstance);

	}
	return pInstance;
}

CGameObject* CMainCamera::Clone(void* pArg)
{
	CMainCamera* pInstance = new CMainCamera(*this);
	if (FAILED(pInstance->Initialize_Copytype(pArg)))
	{
		MSG_BOX("Failed to Cloned :CMainCamera ");
		Safe_Release(pInstance);

	}
	return pInstance;
}

void CMainCamera::Free()
{
	Safe_Release(m_pPerspectiveCameraCom);

	__super::Free();
	

}

#include "CPostProcessCamera.h"
#include "CGameInstance.h"

CPostProcessCamera::CPostProcessCamera(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CCamera_Base(pDevice,pContext)
{
}

CPostProcessCamera::CPostProcessCamera(const CPostProcessCamera& rhs)
    :CCamera_Base(rhs)
{
}

HRESULT CPostProcessCamera::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CPostProcessCamera::Initialize_Copytype(void* pArg)
{
    if (FAILED(__super::Initialize_Copytype(pArg)))
        return E_FAIL;

    CComponent* pOrtho = dynamic_cast<CComponent*>(m_pGameInstance->Clone_Prototype
    (PROTOTYPE::COMPONENT, 0, PROTO_COMPONENT_NAME(L"OrthographicCamera"), pArg));

    if (FAILED(Add_Component(COMPONENT_TYPE::ORTHOGRAPHIC_CAM, pOrtho, (CComponent**)(&m_pCameraCom))))
        return E_FAIL;

    return S_OK;
}

void CPostProcessCamera::Update_Priority(_float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);






}

void CPostProcessCamera::Update(_float fTimeDelta)
{
}

void CPostProcessCamera::Update_Late(_float fTimeDelta)
{
}

void CPostProcessCamera::Update_Render(_float fTimeDelta)
{
}

HRESULT CPostProcessCamera::Render()
{
    return S_OK;
}

CPostProcessCamera* CPostProcessCamera::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
{
    CPostProcessCamera* pInstance = new CPostProcessCamera(_pDevice, _pDeviceContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create :CPostProcessCamera ");
        Safe_Release(pInstance);

    }
    return pInstance;
}

CGameObject* CPostProcessCamera::Clone(void* pArg)
{
    CPostProcessCamera* pInstance = new CPostProcessCamera(*this);
    if (FAILED(pInstance->Initialize_Copytype(pArg)))
    {
        MSG_BOX("Failed to Cloned :CPostProcessCamera ");
        Safe_Release(pInstance);

    }
    return pInstance;
}

void CPostProcessCamera::Free()
{
    __super::Free();
}

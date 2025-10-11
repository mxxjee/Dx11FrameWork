#include "CFreeCamera.h"
#include "CPerspectiveCameraComponent.h"
#include "CGameInstance.h"
#include "CInput_Manager.h"
#include "CShader.h"


CFreeCamera::CFreeCamera(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CCamera_Base(pDevice,pContext)
{
}

CFreeCamera::CFreeCamera(const CFreeCamera& rhs)
    : CCamera_Base(rhs), m_pPerspectiveCameraCom(rhs.m_pPerspectiveCameraCom)
{
}

HRESULT CFreeCamera::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CFreeCamera::Initialize_Copytype(void* pArg)
{
    if (FAILED(__super::Initialize_Copytype(pArg)))
        return E_FAIL;

    m_ScreenHeight = m_pGameInstance->Get_EngineDesc().iWinSizeY;
    m_ScreenWidth = m_pGameInstance->Get_EngineDesc().iWinSizeX;

    GAMEOBJECT_DESC* pDesc = static_cast<GAMEOBJECT_DESC*>(pArg);

    m_pPerspectiveCameraCom = dynamic_cast<CPerspectiveCameraComponent*>(m_pGameInstance->Clone_Prototype
    (PROTOTYPE::COMPONENT, 0, PROTO_COMPONENT_NAME(L"PerspectiveCamera"), pDesc));

    CheckNullResult(m_pPerspectiveCameraCom, E_FAIL);

    Safe_AddRef(m_pPerspectiveCameraCom);
    m_Components.emplace(L"PerspectiveCamera", m_pPerspectiveCameraCom);



    return S_OK;
}

void CFreeCamera::Update_Priority(_float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);

}

void CFreeCamera::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);

    Mouse_Move();
   // Mouse_Fix();

    if (CInput_Manager::GetInstance()->IsKeyHeld(KeyCode::D))
        m_pTransformCom->Move(DIRECTION::RIGHT, fTimeDelta);

    if (CInput_Manager::GetInstance()->IsKeyHeld(KeyCode::A))
        m_pTransformCom->Move(DIRECTION::LEFT, fTimeDelta);

    if (CInput_Manager::GetInstance()->IsKeyHeld(KeyCode::W))
        m_pTransformCom->Move(DIRECTION::FORWARD, fTimeDelta);

    if (CInput_Manager::GetInstance()->IsKeyHeld(KeyCode::S))
        m_pTransformCom->Move(DIRECTION::BACKWARD, fTimeDelta);
}

void CFreeCamera::Update_Late(_float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);

    m_pPerspectiveCameraCom->Update_ViewMatrix(fTimeDelta);

}

void CFreeCamera::Update_Render(_float fTimeDelta)
{
}

HRESULT CFreeCamera::Render()
{
    return S_OK;
}

void CFreeCamera::Bind_ViewProjMatrix()
{
    //카메라 view/투영 세팅
    if (m_pMainShader)
    {
        _float4x4 viewproj;
        XMStoreFloat4x4(&viewproj, m_pPerspectiveCameraCom->Get_MulViewProjMatrix());
        m_pMainShader->SetMatrix("g_ViewProjMatrix", viewproj);
    }
}

void CFreeCamera::Mouse_Move()
{
    long MouseMove = {};

    //오른쪽 클릭하고있을때만 활성화
    CheckFalse(CInput_Manager::GetInstance()->IsMouseButtonHeld(1))

    if (MouseMove= CInput_Manager::GetInstance()->GetMouseDelta().x)
    {
        int A = MouseMove;
        m_pTransformCom->AddRotation(_float3(0.f, (MouseMove / 10.f), 0.f));
        
    }

    if (MouseMove= CInput_Manager::GetInstance()->GetMouseDelta().y)
    {
        int A = MouseMove;
        m_pTransformCom->AddRotation(_float3((MouseMove / 10.f), 0.f, 0.f));
        
    }
}

void CFreeCamera::Mouse_Fix()
{
    POINT	ptMouse{ m_ScreenWidth>>1, m_ScreenHeight >> 1 };
    ClientToScreen(m_pGameInstance->Get_EngineDesc().hWnd, &ptMouse);
    SetCursorPos(ptMouse.x, ptMouse.y);

}

CFreeCamera* CFreeCamera::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
{
    CFreeCamera* pInstance = new CFreeCamera(_pDevice, _pDeviceContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create :CFreeCamera ");
        Safe_Release(pInstance);

    }
    return pInstance;
}

CGameObject* CFreeCamera::Clone(void* pArg)
{
    CFreeCamera* pInstance = new CFreeCamera(*this);
    if (FAILED(pInstance->Initialize_Copytype(pArg)))
    {
        MSG_BOX("Failed to Cloned :CFreeCamera ");
        Safe_Release(pInstance);

    }
    return pInstance;
}

void CFreeCamera::Free()
{
    Safe_Release(m_pPerspectiveCameraCom);
    __super::Free();

}

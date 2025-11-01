#include "CModelObject.h"
#include "CGameInstance.h"
#include "CModel.h"
#include "CShader.h"
#include "CInput_Manager.h"
#include "CMeshComponent.h"
#include "CModel.h"


CModelObject::CModelObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CGameObject(pDevice,pContext), m_pInputManager(CInput_Manager::GetInstance())
{
    Safe_AddRef(m_pInputManager);
}

CModelObject::CModelObject(const CModelObject& rhs)
    :CGameObject(rhs), m_pInputManager(rhs.m_pInputManager)
{
    Safe_AddRef(m_pInputManager);
}

HRESULT CModelObject::Initialize_Prototype()
{

    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CModelObject::Initialize_Copytype(void* pArg)
{
    /*부모 컴포넌트 값세팅 */
    if (FAILED(__super::Initialize_Copytype(pArg)))
        return E_FAIL;

    /*내 컴포넌트 값 세팅*/
    if (FAILED(Ready_Components(pArg)))
        return E_FAIL;

    /*내 컴포넌트 값 세팅*/
    if (FAILED(Ready_Resource(pArg)))
        return E_FAIL;

 
    return S_OK;

}

void CModelObject::Update_Priority(_float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
}

void CModelObject::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
    CheckNull(m_pTransformCom);

    Move_Input(fTimeDelta);
}

void CModelObject::Update_Late(_float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);
}

void CModelObject::Update_Render(_float fTimeDelta)
{
    __super::Update_Render(fTimeDelta);
   m_pGameInstance->Add_RenderObject(m_eRenderGroup, this);

}

HRESULT CModelObject::Render()
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
        
            if (FAILED(m_pShader->Begin(Mesh.second->Get_PassName())))
                return E_FAIL;

            if (FAILED(m_pModel->Render(Mesh.second)))
                return E_FAIL;

        
        }



    }


    return S_OK;
}

HRESULT CModelObject::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShader, "g_WorldMatrix")))
        return E_FAIL;



    return S_OK;
}



CModelObject* CModelObject::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
{
    CModelObject* pInstance = new CModelObject(_pDevice, _pDeviceContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create :CModelObject ");
        Safe_Release(pInstance);

    }



    return pInstance;
}

CGameObject* CModelObject::Clone(void* pArg)
{
    CModelObject* pInstance = new CModelObject(*this);
    if (FAILED(pInstance->Initialize_Copytype(pArg)))
    {
        MSG_BOX("Failed to Cloned :CModelObject ");
        Safe_Release(pInstance);

    }
    return pInstance;
}


HRESULT CModelObject::Ready_Components(void* pArg)
{
    CheckNullResult(pArg, E_FAIL);
    MODELOBJECT_DESC* pModelDesc = static_cast<MODELOBJECT_DESC*>(pArg);
    if (pModelDesc)
    {
        CModel::MODEL_DSC* ppModelDesc = static_cast<CModel::MODEL_DSC*>(pModelDesc->modelDesc);
        m_pModel = m_pGameInstance->Clone_Model(pModelDesc->modelName, ppModelDesc);
        
        if (!m_pModel)
            return E_FAIL;
    }
   
    return S_OK;
}

HRESULT CModelObject::Ready_Resource(void* pArg)
{
    CheckNullResult(pArg, E_FAIL);
    MODELOBJECT_DESC* pModel_dsc = static_cast<MODELOBJECT_DESC*>(pArg);

    m_eRenderGroup = pModel_dsc->eRenderGroup;



    if (m_pModel)
    {
        m_pShader = m_pModel->Get_Shader();
        Safe_AddRef(m_pShader);

    }
    


    return S_OK;
}

void CModelObject::Free()
{
    __super::Free();
    Safe_Release(m_pInputManager);
    Safe_Release(m_pShader);
    Safe_Release(m_pModel);

}

void CModelObject::Move_Input(float fTimeDelta)
{
    if (CInput_Manager::GetInstance()->IsKeyHeld(KeyCode::Q))
    {
        m_pTransformCom->AddRotation(_float3(0.f, 10.f, 0.f));

    }
    if (CInput_Manager::GetInstance()->IsKeyHeld(KeyCode::E))
    {
        m_pTransformCom->AddRotation(_float3(0.f, -10.f, 0.f));

    }

    if (CInput_Manager::GetInstance()->IsKeyHeld(KeyCode::Z))
    {
        m_pTransformCom->AddRotation(_float3(10.f, 0.f, 0.f));

    }
    if (CInput_Manager::GetInstance()->IsKeyHeld(KeyCode::C))
    {
        m_pTransformCom->AddRotation(_float3(-10.f, 0.f, 0.f));

    }

    if (CInput_Manager::GetInstance()->IsKeyHeld(KeyCode::RightArrow))
    {
        bPressed = true;

        if (m_pInputManager->IsKeyHeld(KeyCode::UpArrow))
            m_pTransformCom->Rotation(_float3(0.f, 135.f, 0.f));

        else if (m_pInputManager->IsKeyHeld(KeyCode::DownArrow))
            m_pTransformCom->Rotation(_float3(0.f, 45.f, 0.f));
        else
            m_pTransformCom->Rotation(_float3(0.f, 90.f, 0.f));
    }



    else if (CInput_Manager::GetInstance()->IsKeyHeld(KeyCode::LeftArrow))
    {
        bPressed = true;

        if (m_pInputManager->IsKeyHeld(KeyCode::UpArrow))
            m_pTransformCom->Rotation(_float3(0.f, -135.f, 0.f));

        else if (m_pInputManager->IsKeyHeld(KeyCode::DownArrow))
            m_pTransformCom->Rotation(_float3(0.f, -45.f, 0.f));
        else
            m_pTransformCom->Rotation(_float3(0.f, -90.f, 0.f));
    }


    else if (CInput_Manager::GetInstance()->IsKeyHeld(KeyCode::UpArrow))
    {
        bPressed = true;
        m_pTransformCom->Rotation(_float3(0.f, 0.f, 0.f));
    }


    else if (CInput_Manager::GetInstance()->IsKeyHeld(KeyCode::DownArrow))
    {
        bPressed = true;
        m_pTransformCom->Rotation(_float3(0.f, 180.f, 0.f));
    }


    if (m_pInputManager->IsKeyReleased(KeyCode::UpArrow) ||
        m_pInputManager->IsKeyReleased(KeyCode::DownArrow) ||
        m_pInputManager->IsKeyReleased(KeyCode::LeftArrow) ||
        m_pInputManager->IsKeyReleased(KeyCode::RightArrow))
        bPressed = false;

    if (bPressed)
        m_pTransformCom->Move(DIRECTION::FORWARD,  fTimeDelta);


    if (m_pTarget)
        m_pTransformCom->Chase(m_pTarget->Get_Transform()->Get_State(STATE::POSITION, TransformScope::WORLD), fTimeDelta, 5);

}

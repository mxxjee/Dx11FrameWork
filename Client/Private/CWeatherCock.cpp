#include "CWeatherCock.h"
#include "CBoxColliderComponent.h"
#include "CCollider_Base.h"
#include "CBounding_AABB.h"
#include "Client_Defines.h"

#include "CAnimBody.h"
#include "CModel.h"




USING(Client)

CWeatherCock::CWeatherCock(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CAnimModelObject(pDevice, pContext)
{
}

CWeatherCock::CWeatherCock(const CWeatherCock& rhs)
    :CAnimModelObject(rhs)
{
}

HRESULT CWeatherCock::Initialize_Prototype(void* pArg)
{
    if (FAILED(__super::Initialize_Copytype(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components(pArg)))
        return E_FAIL;

    if (FAILED(Ready_PartObjects(pArg)))
        return E_FAIL;



    m_pAnimBody->Get_Model()->Set_Animation(L"wait",true);

    return S_OK;
}


void CWeatherCock::Update_Priority(_float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
}

void CWeatherCock::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CWeatherCock::Update_Late(_float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);
}

void CWeatherCock::Update_Render(_float fTimeDelta)
{
    __super::Update_Render(fTimeDelta);
}

HRESULT CWeatherCock::Render()
{
 
    return S_OK;
}

HRESULT CWeatherCock::Ready_Components(void* pArg)
{
    //////////////Boxcollider√ﬂ∞°

    CCollider_Base::COLLIDER_DESC pColliderDesc;
    pColliderDesc.m_eColGroup = ENUM_TO_UINT(COLLISION_GROUP::OBJECT);
    pColliderDesc.m_iLevelID = ENUM_TO_UINT(LEVEL_ID::STATIC);

    CBounding_AABB::BOUNDING_AABB_DESC CollDesc;
    CollDesc.vCenter = { 0.f,0.5f,0.f };
    CollDesc.Extents = { 0.3f,0.8f,0.4f };
    pColliderDesc.m_BoundingDesc = &CollDesc;


    CComponent* pCollider = dynamic_cast<CBoxColliderComponent*>(m_pGameInstance->Clone_Prototype(
        PROTOTYPE::COMPONENT,
        0,
        PROTO_COMPONENT_NAME(L"BoxCollider"),
        &pColliderDesc)
        );

    if (FAILED(Add_Component(
        COMPONENT_TYPE::BOX_COLLIDER,
        pCollider,
        reinterpret_cast<CComponent**>(&m_pCollider)
    )))
        return E_FAIL;

    m_pCollider->Set_Trigger(false);
    return S_OK;
}

HRESULT CWeatherCock::Ready_PartObjects(void* pArg)
{
    CheckNullResult(pArg, E_FAIL);
    MODELOBJECT_DESC* pModelDesc = static_cast<MODELOBJECT_DESC*>(pArg);
    if (pModelDesc)
    {
        CAnimBody::ANIMBODY_DESC pBodyDesc;

        pBodyDesc.eRenderGroup = ENUM_TO_UINT(RENDERGROUP::NONALPHA);
        pBodyDesc.modelName = L"Weathercock";
        pBodyDesc.pOwner = this;
        pBodyDesc.m_iLevelID = ENUM_TO_UINT(LEVEL_ID::STATIC);
        pBodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
  

        if (FAILED(__super::Add_PartObject(0, PROTO_OBJ_NAME(L"AnimBody"), L"Part_Body", &pBodyDesc)))
            return E_FAIL;

        m_pBody = dynamic_cast<CBody*>(Find_PartObject(L"Part_Body"));

        if (m_pBody)
            m_pAnimBody = dynamic_cast<CAnimBody*>(m_pBody);
    }

    return S_OK;
}

CWeatherCock* CWeatherCock::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContex, void* pArg)
{
    CWeatherCock* pInstance = new CWeatherCock(_pDevice, _pDeviceContex);
    if (FAILED(pInstance->Initialize_Prototype(pArg)))
    {
        MSG_BOX("Failed to Create :CWeatherCock ");
        Safe_Release(pInstance);

    }

    return pInstance;
}



void CWeatherCock::Free()
{
    __super::Free();
    Safe_Release(m_pCollider);
}

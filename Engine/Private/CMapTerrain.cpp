#include "CMapTerrain.h"
#include "CModel.h"
#include "CShader.h"
#include "CMeshComponent.h"
#include "CGameInstance.h"
#include "CGameInstance.h"
#include "CMeshColliderComponent.h"
#include "MathUtils.h"


CMapTerrain::CMapTerrain(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CTerrain_Base(pDevice,pContext)
{
}

CMapTerrain::CMapTerrain(const CMapTerrain& rhs)
    :CTerrain_Base(rhs)
{
}

HRESULT CMapTerrain::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    

    return S_OK;
}

HRESULT CMapTerrain::Initialize_Copytype(void* pArg)
{
    /*부모 컴포넌트 값세팅 */
    if (FAILED(CGameObject::Initialize_Copytype(pArg)))
        return E_FAIL;

    /*내 컴포넌트 값 세팅*/
    if (FAILED(Ready_Components(pArg)))
        return E_FAIL;

    /*내 컴포넌트 값 세팅*/
    if (FAILED(Ready_Resources(pArg)))
        return E_FAIL;


    m_eObjType = MapObjType::TERRAIN;


    m_passName = "Default";

    return S_OK;
}

void CMapTerrain::Update_Priority(_float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
}

void CMapTerrain::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
    CheckNull(m_pTransformCom);
    CheckNull(m_pMeshCollidercomponent);

    m_pMeshCollidercomponent->Update_Collider(m_pTransformCom);

    
}

void CMapTerrain::Update_Late(_float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);
}

void CMapTerrain::Update_Render(_float fTimeDelta)
{
    __super::Update_Render(fTimeDelta);
}

HRESULT CMapTerrain::Render()
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

            if (FAILED(m_pShader->Begin(m_passName)))
                return E_FAIL;

            if (FAILED(m_pModel->Render(Mesh.second)))
                return E_FAIL;


        }



    }


    return S_OK;
}

HRESULT CMapTerrain::Ready_Components(void* pArg)
{
    CheckNullResult(pArg, E_FAIL);
    MAPTERRAIN_DESC* pTerrainModelDesc = static_cast<MAPTERRAIN_DESC*>(pArg);
    if (pTerrainModelDesc)
    {
        CModel::MODEL_DSC* ppModelDesc = static_cast<CModel::MODEL_DSC*>(pTerrainModelDesc->modelDesc);
        ppModelDesc->pOwner = this;
        m_pModel = m_pGameInstance->Clone_Model(pTerrainModelDesc->modelName, ppModelDesc);

        if (!m_pModel)
            return E_FAIL;


        
        /*메쉬콜라이더컴포넌트(픽킹검사)*/
        CMeshColliderComponent::COLLIDER_MESH ColliderDesc;
        ColliderDesc.pModel = m_pModel;
        ColliderDesc.pOwner = this;
        ColliderDesc.vScaleOffSet = _float3(100.f, 100.f, 100.f);

        CComponent* pMeshCollider = dynamic_cast<CMeshColliderComponent*>(m_pGameInstance->Clone_Prototype(
            PROTOTYPE::COMPONENT,
            0,
            PROTO_COMPONENT_NAME(L"MeshCollider"),
            &ColliderDesc));

        if (FAILED(Add_Component(
            COMPONENT_TYPE::MESH_COLLIDER,
            pMeshCollider,
            reinterpret_cast<CComponent**>(&m_pMeshCollidercomponent)
        )))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CMapTerrain::Ready_Resources(void* pArg)
{

    CheckNullResult(pArg, E_FAIL);
    MAPTERRAIN_DESC* pTerrainModelDesc = static_cast<MAPTERRAIN_DESC*>(pArg);



	if (m_pShader)
		Safe_Release(m_pShader);

    m_pShader = m_pGameInstance->Find_Shader(L"Shader_MapTool");
	Safe_AddRef(m_pShader);




    return S_OK;
}

HRESULT CMapTerrain::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShader, "g_WorldMatrix")))
        return E_FAIL;


    return S_OK;
}

CMapTerrain* CMapTerrain::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
{
    CMapTerrain* pInstance = new CMapTerrain(_pDevice, _pDeviceContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create :CMapTerrain ");
        Safe_Release(pInstance);

    }



    return pInstance;
}

CGameObject* CMapTerrain::Clone(void* pArg)
{
    CMapTerrain* pInstance = new CMapTerrain(*this);
    if (FAILED(pInstance->Initialize_Copytype(pArg)))
    {
        MSG_BOX("Failed to Cloned :CMapModel ");
        Safe_Release(pInstance);

    }
    return pInstance;
}

void CMapTerrain::Free()
{
    __super::Free();
    Safe_Release(m_pMeshCollidercomponent);
    Safe_Release(m_pModel);
}

bool CMapTerrain::Is_Picked(_vector Origin, _vector Dir, float& Dist)
{
    CheckNullResult(m_pMeshCollidercomponent, false);

    bool Result = m_pMeshCollidercomponent->Intersects_Ray(Origin, Dir, Dist);


    return Result;
}

void CMapTerrain::OnSeletected(bool bSelected)
{
    m_bSelected = bSelected;

    if (m_bSelected)
        m_passName = "Select";

    else
        m_passName = "Default";

}

void CMapTerrain::Save_To_Json(json& Json)
{
    json Meta;
    Meta["TileID"] = m_iIdx;
    wstring name= m_pModel->Get_ModelData().name;
 
    Meta["ModelName"] = WStringToUTF8(name);


    json Transform;
    //Transform정보
    _float3 s, t;
    _float4 r;


    XMStoreFloat3(&s, m_pTransformCom->Get_SRT(SRTType::SCALE));
    XMStoreFloat3(&t, m_pTransformCom->Get_SRT(SRTType::TRANSFORM));
    XMStoreFloat4(&r, m_pTransformCom->Get_SRT(SRTType::ROTATION));
    _float3 rResult = MathUtils::QuaternionToEuler(XMLoadFloat4(&r));

    json position=json::array();
    position.push_back(t.x);
    position.push_back(t.y);
    position.push_back(t.z);
    Transform["Position"] = position;



    json scale = json::array();
    scale.push_back(s.x);
    scale.push_back(s.y);
    scale.push_back(s.z);
    Transform["Scale"] = scale;



    json Rotation = json::array();
    Rotation.push_back(r.x);
    Rotation.push_back(r.y);
    Rotation.push_back(r.z);
    Transform["Rotation"] = Rotation;

    Meta["Transform"]=Transform;

    Json.push_back(Meta);
}

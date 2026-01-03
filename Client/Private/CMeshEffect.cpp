#include "CMeshEffect.h"
#include "CGameInstance.h"
#include "CModel.h"
#include "CMeshComponent.h"
#include "CShader.h"
#include "CEffectData_Manager.h"



USING(Client)

CMeshEffect::CMeshEffect(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CGameObject(pDevice,pContext)
{
}

CMeshEffect::CMeshEffect(const CMeshEffect& rhs)
    : CGameObject(rhs)
{
}

HRESULT CMeshEffect::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CMeshEffect::Initialize_Copytype(void* pArg)
{
    


    if (FAILED(__super::Initialize_Copytype(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Resource(pArg)))
        return E_FAIL;


    m_pEffectData_Manager = CEffectData_Manager::GetInstance();
    EffectData* pData=m_pEffectData_Manager->Find_Data(m_pModel->Get_ModelData().name);
    if (pData)
        m_LocalData = *pData;

    return S_OK;
}

void CMeshEffect::Update_Priority(_float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
}

void CMeshEffect::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);

}

void CMeshEffect::Update_Late(_float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);
    m_fTime += fTimeDelta * m_LocalData.fSpeed;
    if (m_fTime >= m_LocalData.fLifeTime)
    {
        m_fTime = 0.f;

    }

}

void CMeshEffect::Update_Render(_float fTimeDelta)
{
    __super::Update_Render(fTimeDelta);
    m_pGameInstance->Add_RenderObject(ENUM_TO_UINT(RENDERGROUP::NONALPHA), this);
}

HRESULT CMeshEffect::Render()
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
            Mesh.second->Set_PassName("MeshEffect");
            Mesh.second->Bind_ShaderResource(m_pShader, "g_DiffuseTexture", aiTextureType::aiTextureType_DIFFUSE);
      
            if (FAILED(m_pShader->Begin(Mesh.second->Get_PassName())))
                return E_FAIL;

            if (FAILED(m_pModel->Render(Mesh.second)))
                return E_FAIL;


        }



    }


    return S_OK;
}

void CMeshEffect::Set_VisibleMesh(const wstring& MeshName, bool bVisible)
{
    if (m_pModel)
        m_pModel->Set_VisibleMesh(MeshName, bVisible);
}

HRESULT CMeshEffect::Ready_Components(void* pArg)
{
    CheckNullResult(pArg, E_FAIL);
    MESHEFFECT_DESC* pDesc = static_cast<MESHEFFECT_DESC*>(pArg);

    if (pDesc)
    {
        CModel::MODEL_DSC pModelDesc;
        pModelDesc.pOwner = this;
        m_pModel = m_pGameInstance->Clone_Model(pDesc->modelName, &pModelDesc);

        if (!m_pModel)
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CMeshEffect::Ready_Resource(void* pArg)
{
    if (m_pModel)
    {
        m_pShader = m_pModel->Get_Shader();
        Safe_AddRef(m_pShader);

        m_ModelName = m_pModel->Get_ModelData().name;

    }
    return S_OK;
}

HRESULT CMeshEffect::Bind_ShaderResources()
{
    /*Combined Matrix를 직접던진다.*/
    if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", *m_pTransformCom->Get_WorldMatrixPtr())))
        return E_FAIL;

    if (FAILED(m_pShader->Bind_Vector("g_TintColor", m_LocalData.vColor)))
        return E_FAIL;

    _float4 vTime = _float4(m_fTime, 0.f, 0.f, 0.f);
    m_pGameInstance->CopyData_Buffer("LoadingBuffer", &vTime, sizeof(_float4));



    return S_OK;
}

CMeshEffect* CMeshEffect::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
{
    CMeshEffect* pInstance = new CMeshEffect(_pDevice, _pDeviceContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CMeshEffect");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CMeshEffect::Clone(void* pArg)
{
    CMeshEffect* pInstance = new CMeshEffect(*this);

    if (FAILED(pInstance->Initialize_Copytype(pArg)))
    {
        MSG_BOX("Failed to Created : CMeshEffect");
        Safe_Release(pInstance);
    }
    return pInstance;
}
#ifdef _DEBUG
void CMeshEffect::Render_DebugImgui()
{
    __super::Render_DebugImgui();

    if(ImGui::ColorEdit4("Color", (float*)&m_LocalData.vColor))
    {
        
    }

    if (ImGui::DragFloat("LifeTime", (float*)&m_LocalData.fLifeTime))
    {
        m_pEffectData_Manager->Update_Data(m_ModelName, m_LocalData);

    }

    if (ImGui::DragFloat("Speed", (float*)&m_LocalData.fSpeed))
    {
        m_pEffectData_Manager->Update_Data(m_ModelName, m_LocalData);

    }

    if(ImGui::Button("Save"))
    {
        wstring name = m_pModel->Get_ModelData().name;
        m_pEffectData_Manager->Save_To_Json(m_ModelName, m_LocalData);


    }

}
#endif // _DEBUG



void CMeshEffect::Free()
{
    __super::Free();


    Safe_Release(m_pModel);
    Safe_Release(m_pShader);

}



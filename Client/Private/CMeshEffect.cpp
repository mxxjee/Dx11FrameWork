#include "CMeshEffect.h"
#include "CGameInstance.h"
#include "CModel.h"
#include "CMeshComponent.h"
#include "CShader.h"
#include "CEffectData_Manager.h"



USING(Client)

CMeshEffect::CMeshEffect(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CEffect(pDevice,pContext)
{
}

CMeshEffect::CMeshEffect(const CMeshEffect& rhs)
    : CEffect(rhs)
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


    EffectData* pData=m_pEffectData_Manager->Find_Data(m_pModel->Get_ModelData().name);
    if (pData)
    {
        m_LocalData = *pData;
        m_pTransformCom->Rotation(_float3(m_LocalData.InitRotation.x, m_LocalData.InitRotation.y, m_LocalData.InitRotation.z));
        m_pTransformCom->Set_Scale(m_LocalData.InitScale);

        _vector vPos = m_pTransformCom->Get_State(STATE::POSITION);
        m_pTransformCom->Set_State(STATE::POSITION,vPos+XMVectorSetW(XMLoadFloat4(&m_LocalData.InitOffSet),0.f));


       
    }
    /*spawn시에 필요한 행렬만들어주기*/

  //  Update_Matrix();
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

    if(!m_bStop)
        m_fTime += fTimeDelta * m_LocalData.fSpeed;


    if (m_fTime >= m_LocalData.fLifeTime)
    {
        if (!m_LocalData.m_bLoop)
            Set_Active(false);

        m_fTime = 0.f;
        
        
    }


}

void CMeshEffect::Update_Render(_float fTimeDelta)
{
    __super::Update_Render(fTimeDelta);
  

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
            Mesh.second->Set_PassName(m_PassName);
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
    MESHEFFECT_DESC* pDesc = static_cast<MESHEFFECT_DESC*>(pArg);
    m_PassName = pDesc->PassName;

    if (m_pModel)
    {
        m_pModel->Set_Shader(m_pGameInstance->Find_Shader(L"MeshEffect"));

        if (pDesc->ShaderName != L"")
            m_pModel->Set_Shader(m_pGameInstance->Find_Shader(pDesc->ShaderName));


        m_pShader = m_pModel->Get_Shader();
        Safe_AddRef(m_pShader);
        m_ModelName = m_pModel->Get_ModelData().name;

    }


    for (auto& Mesh : m_pModel->Get_Meshs())
    {
        /*모든 메쉬를 순회하면서 바인드한다.
           각 메쉬들의 위치와 소유한 메테리얼의 이미지 바인딩.
           이후 메쉬를 그리는 작업*/

        if (Mesh.second)
            Mesh.second->Set_PassName(pDesc->PassName);
      
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
        m_pEffectData_Manager->Update_Data(m_ModelName, m_LocalData);

    }

    if (ImGui::DragFloat4("InitOffSet", (float*)&m_LocalData.InitOffSet))
    {
        m_pEffectData_Manager->Update_Data(m_ModelName, m_LocalData);
        m_pTransformCom->Set_State(STATE::POSITION,
            m_LocalData.InitOffSet);
        //Update_Matrix();
    }

    if (ImGui::DragFloat4("InitRotation", (float*)&m_LocalData.InitRotation))
    {
        m_pEffectData_Manager->Update_Data(m_ModelName, m_LocalData);
        m_pTransformCom->Rotation(
            _float3(m_LocalData.InitRotation.x,
                m_LocalData.InitRotation.y,
                m_LocalData.InitRotation.z));
       // Update_Matrix();
    }

    if (ImGui::DragFloat4("InitScale", (float*)&m_LocalData.InitScale))
    {
        m_pEffectData_Manager->Update_Data(m_ModelName, m_LocalData);
        m_pTransformCom->Set_Scale(m_LocalData.InitScale);
       // Update_Matrix();
    }

    if (ImGui::DragFloat("LifeTime", (float*)&m_LocalData.fLifeTime))
    {
        m_pEffectData_Manager->Update_Data(m_ModelName, m_LocalData);

    }

    if (ImGui::DragFloat("Speed", (float*)&m_LocalData.fSpeed))
    {
        m_pEffectData_Manager->Update_Data(m_ModelName, m_LocalData);

    }

    if (ImGui::Checkbox("Loop", (bool*)&m_LocalData.m_bLoop))
    {
        m_pEffectData_Manager->Update_Data(m_ModelName, m_LocalData);

    }

    if (ImGui::Button("Play"))
    {
        Play();


    }
    if (ImGui::Button("Stop"))
        Stop();


    if(ImGui::Button("Save"))
    {
        wstring name = m_pModel->Get_ModelData().name;
        m_pEffectData_Manager->Save_To_Json(m_ModelName, m_LocalData);


    }

}
#endif // _DEBUG



void CMeshEffect::Free()
{
    Safe_Release(m_pModel);

    __super::Free();



    

}

void CMeshEffect::Spawn(const _float4x4* pSocketMatrix, const _float4x4* pParentMatrix)
{
    
    _uint iCurrentLevelID = m_pGameInstance->Get_CurrentLevelID();


    //레벨이 달라졋을때만 레이어에넣기
    if (iCurrentLevelID != m_iSceneID)
    {
        m_iSceneID = iCurrentLevelID;
        m_pGameInstance->Add_GameObject_To_Layer(m_iSceneID, L"Particle_Layer", this);

    }


}



void CMeshEffect::Play()
{
    if (!m_bActive)
        Set_Active(true);

    ///InitData
    m_bStop = false;
    m_fTime = 0.f;
}

void CMeshEffect::Stop()
{
    m_bStop = true;
}




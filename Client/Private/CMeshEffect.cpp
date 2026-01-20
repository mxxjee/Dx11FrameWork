#include "CMeshEffect.h"
#include "CGameInstance.h"
#include "CModel.h"
#include "CMeshComponent.h"
#include "CShader.h"
#include "CEffectData_Manager.h"
#include "CEffectPoolManager.h"
#include "CLayer.h"




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
    MESHEFFECT_DESC* pDesc = static_cast<MESHEFFECT_DESC*>(pArg);
    m_DataName = pDesc->modelName;

    if (FAILED(__super::Initialize_Copytype(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components(pArg)))
        return E_FAIL;

    if (FAILED(Ready_Resource(pArg)))
        return E_FAIL;




    EffectData* pData = m_pEffectData_Manager->Find_Data(m_DataName);
    if (pData)
    {
        m_LocalData = *(static_cast<MeshEffectData*>(pData));
        m_pTransformCom->Rotation(_float3(m_LocalData.InitRotation.x, m_LocalData.InitRotation.y, m_LocalData.InitRotation.z));
        m_pTransformCom->Set_Scale(m_LocalData.InitScale);

        _vector vPos = m_pTransformCom->Get_State(STATE::POSITION);
        m_pTransformCom->Set_State(STATE::POSITION, vPos + XMVectorSetW(XMLoadFloat4(&m_LocalData.InitOffSet), 0.f));



    }

    m_pDataRef = &m_LocalData;
    CurrentScale = m_pDataRef->InitScale;
    CurrentMove = m_pDataRef->InitOffSet;
    Make_LocalMatrix();
    return S_OK;
}

void CMeshEffect::Update_Priority(_float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);



}

void CMeshEffect::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);

    bool m_bAnimated = m_LocalData.bUseMoveAnim || m_LocalData.bUseScaleAnim || m_LocalData.bUseRotationAnim;

    if (m_LocalData.bUseScaleAnim)
    {
        //진행도 누적
        ScaleLerpTime += fTimeDelta * m_LocalData.fScaleSpeed;
        if (ScaleLerpTime > 1.f) ScaleLerpTime = 1.f;

        //보간 값 계산
        XMStoreFloat4(&CurrentScale, XMVectorLerp(
            XMLoadFloat4(&m_LocalData.InitScale),
            XMLoadFloat4(&m_LocalData.TargetScale),
            ScaleLerpTime));

        //루프 처리 (끝나면 다시 0으로)
        if (ScaleLerpTime >= 1.f && m_LocalData.m_bLoop)
            ScaleLerpTime = 0.f;



    }

    if (m_LocalData.bUseMoveAnim)
    {
        //진행도 누적
        MoveLerpTime += fTimeDelta * m_LocalData.fMoveSpeed;
        if (MoveLerpTime > 1.f) MoveLerpTime = 1.f;

        //보간 값 계산


        _vector vMoveDir = XMVector4Normalize(XMLoadFloat4(&m_LocalData.vMoveDir));

        //originmarix가있다면 그 look방향 * 내가갈방향
        if (!XMMatrixIsIdentity(XMLoadFloat4x4(&OriginMatrix)))
        {
            _vector vLook = XMLoadFloat4x4(&OriginMatrix).r[ENUM_TO_UINT(STATE::LOOK)];
            vMoveDir = vLook * XMVector4Normalize(XMLoadFloat4(&m_LocalData.vMoveDir));

        }

        XMStoreFloat4(&CurrentMove, XMVectorLerp(XMLoadFloat4(&m_LocalData.InitOffSet),
            XMLoadFloat4(&m_LocalData.InitOffSet) * vMoveDir,
            MoveLerpTime));


        //루프 처리 (끝나면 다시 0으로)
        if (MoveLerpTime >= 1.f && m_LocalData.m_bLoop)
            MoveLerpTime = 0.f;


    }

    if (!m_bAnimated)
    {
        //애니메이션을 안 쓸 때는 실시간으로 InitScale을 따라가야 ImGui 수정이 반영됨!
        CurrentScale = m_LocalData.InitScale;
        ScaleLerpTime = 0.f;
    }


    
  
}

void CMeshEffect::Update_Late(_float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);
    
    


}

void CMeshEffect::Update_Render(_float fTimeDelta)
{
    __super::Update_Render(fTimeDelta);
  

}

HRESULT CMeshEffect::Render()
{
   
    __super::Render();

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
    if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", CombinedMatrix)))
        return E_FAIL;


    _float4 vTime = _float4(m_fProgress, 0.f, 0.f, 0.f);
    m_pGameInstance->CopyData_Buffer("LoadingBuffer", &vTime, sizeof(_float4));

    if (FAILED(m_pShader->Bind_Float("g_Alpha", m_fAlpha)))
        return E_FAIL;

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
    if (ImGui::Checkbox("bUseScaleAnim", (bool*)&m_LocalData.bUseScaleAnim))
    {
        m_pEffectData_Manager->Update_Data(m_DataName, static_cast<MeshEffectData*>(&m_LocalData));

    }

    if (ImGui::DragFloat4("TargetScale", (float*)&m_LocalData.TargetScale))
    {
        m_pEffectData_Manager->Update_Data(m_DataName, static_cast<MeshEffectData*>(&m_LocalData));
    }


    if (ImGui::DragFloat("ScaleSpeed", (float*)&m_LocalData.fScaleSpeed))
    {
        m_pEffectData_Manager->Update_Data(m_DataName, static_cast<MeshEffectData*>(&m_LocalData));
    }


    ///////////
    if (ImGui::Checkbox("bUseRotationAnim", (bool*)&m_LocalData.bUseRotationAnim))
    {
        m_pEffectData_Manager->Update_Data(m_DataName, static_cast<MeshEffectData*>(&m_LocalData));

    }

    if (ImGui::DragFloat4("RotationAxis", (float*)&m_LocalData.vRotationAxis))
    {
        m_pEffectData_Manager->Update_Data(m_DataName, static_cast<MeshEffectData*>(&m_LocalData));
    }


    if (ImGui::DragFloat("RotationSpeed", (float*)&m_LocalData.fRotationSpeed))
    {
        m_pEffectData_Manager->Update_Data(m_DataName, static_cast<MeshEffectData*>(&m_LocalData));
    }
    //////////////
    if (ImGui::Checkbox("bUseMoveAnim", (bool*)&m_LocalData.bUseMoveAnim))
    {
        m_pEffectData_Manager->Update_Data(m_DataName, static_cast<MeshEffectData*>(&m_LocalData));

    }

    if (ImGui::DragFloat4("MoveDir", (float*)&m_LocalData.vMoveDir))
    {
        m_pEffectData_Manager->Update_Data(m_DataName, static_cast<MeshEffectData*>(&m_LocalData));
    }


    if (ImGui::DragFloat("MoveSpeed", (float*)&m_LocalData.fMoveSpeed))
    {
        m_pEffectData_Manager->Update_Data(m_DataName, static_cast<MeshEffectData*>(&m_LocalData));
    }

    __super::Render_DebugImgui();
    //////////////////////
   
    /////////////

  

}
#endif // _DEBUG



void CMeshEffect::Free()
{
    Safe_Release(m_pModel);

    __super::Free();



    

}

void CMeshEffect::Spawn(const _float4x4* pSocketMatrix, const _float4x4* pParentMatrix)
{
    
  


}



void CMeshEffect::Play()
{
    __super::Play();
    if (!m_bActive)
        Set_Active(true);

    ///InitData
    m_bStop = false;
    m_fTime = 0.f;
    m_fAlpha = 1.f;

    Make_LocalMatrix();

    if (m_pParentMatrix && m_pSocketMatrix)
    {
        //먼저 socket*m_pParentMatrix

        _matrix SocketWorld = XMMatrixMultiply(XMLoadFloat4x4(m_pSocketMatrix),
            XMLoadFloat4x4(m_pParentMatrix));

        XMStoreFloat4x4(&CombinedMatrix, XMMatrixMultiply(LocalMatrix, SocketWorld));
    }

    //ParentMAtrix만있을경우
    else if (m_pParentMatrix)
    {
        XMStoreFloat4x4(&CombinedMatrix, XMMatrixMultiply(LocalMatrix, XMLoadFloat4x4(m_pParentMatrix)));

    }



    else
    {

        XMStoreFloat4x4(&CombinedMatrix, XMMatrixMultiply(LocalMatrix, XMLoadFloat4x4(&OriginMatrix)));

    }

    m_pTransformCom->Set_WorldMatrix(CombinedMatrix);
}

void CMeshEffect::Stop()
{
    __super::Stop();
    m_bStop = true;
    m_fProgress = 0.f;
    m_fTime = 0.f;
    m_fAlpha = 1.f;
}




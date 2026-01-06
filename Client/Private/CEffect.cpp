#include "CEffect.h"
#include "CEffectData_Manager.h"
#include "CShader.h"
#include "CEffectPoolManager.h"


USING(Client)

CEffect::CEffect(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	:CGameObject(pDevice,pContext)
{
}

CEffect::CEffect(const CEffect& rhs)
	: CGameObject(rhs)
{
}


HRESULT CEffect::Render()
{
	__super::Render();
	if (FAILED(m_pShader->Bind_Vector("g_TintColor", m_LocalData.vColor)))
		return E_FAIL;

    if (FAILED(m_pShader->Bind_Float("g_fIntensity", m_LocalData.fIntensity)))
        return E_FAIL;

	return S_OK;
}

void CEffect::Spawn(const _float4x4* pSocketMatrix, const _float4x4* pParentMatrix)
{
    ScaleLerpTime = 0.f;
    MoveLerpTime = 0.f;

    CurrentScale = m_LocalData.InitScale;
    CurrentMove = m_LocalData.InitOffSet;
}

void CEffect::Play()
{
    ScaleLerpTime = 0.f;
    MoveLerpTime = 0.f;

    CurrentScale = m_LocalData.InitScale;
    CurrentMove = m_LocalData.InitOffSet;

}

void CEffect::Stop()
{
    m_fTime = 0.f;
    m_fAlpha = 1.f;
    m_fProgress = 0.f;
    ScaleLerpTime = 0.f;


}

void CEffect::Free()
{
	__super::Free();

	Safe_Release(m_pShader);
}

void CEffect::Make_LocalMatrix()
{
	_matrix Scaling = XMMatrixScaling(CurrentScale.x,
        CurrentScale.y,
        CurrentScale.z);

	_float Roll = XMConvertToRadians(m_LocalData.InitRotation.x);
	_float Pitch = XMConvertToRadians(m_LocalData.InitRotation.y);
	_float Yaw = XMConvertToRadians(m_LocalData.InitRotation.z);

	_matrix Rotation = XMMatrixRotationRollPitchYaw(Roll, Pitch, Yaw);

	_matrix Translate = XMMatrixTranslation(CurrentMove.x,
        CurrentMove.y,
        CurrentMove.z);

	LocalMatrix = Scaling * Rotation * Translate;


}


HRESULT CEffect::Initialize_Prototype()
{
	
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;



	return S_OK;
}

HRESULT CEffect::Initialize_Copytype(void* pArg)
{
	EFFECT_DESC* pDesc = static_cast<EFFECT_DESC*>(pArg);

	m_pEffectData_Manager = CEffectData_Manager::GetInstance();
	m_pEffectPool_Manager = CEffectPoolManager::GetInstance();

	if (FAILED(__super::Initialize_Copytype(pArg)))
		return E_FAIL;
	m_eRenderGroup = pDesc->eRenderGroup;
	m_pShader = m_pGameInstance->Find_Shader(pDesc->ShaderName);
	m_DataName = pDesc->DataName;



    EffectData* pData = m_pEffectData_Manager->Find_Data(m_DataName);
    if (pData)
    {
        m_LocalData = *pData;
        m_pTransformCom->Rotation(_float3(m_LocalData.InitRotation.x, m_LocalData.InitRotation.y, m_LocalData.InitRotation.z));
        m_pTransformCom->Set_Scale(m_LocalData.InitScale);

        _vector vPos = m_pTransformCom->Get_State(STATE::POSITION);
        m_pTransformCom->Set_State(STATE::POSITION, vPos + XMVectorSetW(XMLoadFloat4(&m_LocalData.InitOffSet), 0.f));



    }
    /*spawn시에 필요한 행렬만들어주기*/

  //  Update_Matrix();
 

    CurrentScale = m_LocalData.InitScale;
    CurrentMove = m_LocalData.InitOffSet;
    Make_LocalMatrix();
    return S_OK;
}

void CEffect::Update_Priority(_float fTimeDelta)
{
	__super::Update_Priority(fTimeDelta);
}

void CEffect::Update(_float fTimeDelta)
{
    CheckTrue(m_bStop);

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


        XMStoreFloat4(&CurrentMove, XMVectorLerp(XMLoadFloat4(&m_LocalData.InitOffSet),
            XMLoadFloat4(&m_LocalData.InitOffSet) * XMLoadFloat4(&m_LocalData.vMoveDir),
            MoveLerpTime));


        //루프 처리 (끝나면 다시 0으로)
        if (MoveLerpTime >= 1.f && m_LocalData.m_bLoop)
            MoveLerpTime = 0.f;


    }

    if(!m_bAnimated)
    {
        //애니메이션을 안 쓸 때는 실시간으로 InitScale을 따라가야 ImGui 수정이 반영됨!
        CurrentScale = m_LocalData.InitScale;
        ScaleLerpTime = 0.f;
    }

}

void CEffect::Update_Late(_float fTimeDelta)
{
	__super::Update_Late(fTimeDelta);

    CheckTrue(m_bStop);
    
    
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

        XMStoreFloat4x4(&CombinedMatrix, XMMatrixMultiply(LocalMatrix, OriginMatrix));
        
    }

    m_pTransformCom->Set_WorldMatrix(CombinedMatrix);
}


void CEffect::Render_DebugImgui()
{
	__super::Render_DebugImgui();
    if (ImGui::ColorEdit4("Color", (float*)&m_LocalData.vColor))
    {
        m_pEffectData_Manager->Update_Data(m_DataName, m_LocalData);
        Make_LocalMatrix();
    }

    if (ImGui::DragFloat("Itensity", (float*)&m_LocalData.fIntensity))
    {
        m_pEffectData_Manager->Update_Data(m_DataName, m_LocalData);

    }

    if (ImGui::DragFloat4("InitOffSet", (float*)&m_LocalData.InitOffSet))
    {
        m_pEffectData_Manager->Update_Data(m_DataName, m_LocalData);
        Make_LocalMatrix();
        //Update_Matrix();
    }

    if (ImGui::DragFloat4("InitRotation", (float*)&m_LocalData.InitRotation))
    {
        m_pEffectData_Manager->Update_Data(m_DataName, m_LocalData);
        Make_LocalMatrix();
        // Update_Matrix();
    }

    if (ImGui::DragFloat4("InitScale", (float*)&m_LocalData.InitScale))
    {
        m_pEffectData_Manager->Update_Data(m_DataName, m_LocalData);
        Make_LocalMatrix();
        CurrentScale = m_LocalData.InitScale;

        // Update_Matrix();
    }

    //////////////////////
    if (ImGui::Checkbox("bUseScaleAnim", (bool*)&m_LocalData.bUseScaleAnim))
    {
        m_pEffectData_Manager->Update_Data(m_DataName, m_LocalData);

    }

    if (ImGui::DragFloat4("TargetScale", (float*)&m_LocalData.TargetScale))
    {
        m_pEffectData_Manager->Update_Data(m_DataName, m_LocalData);
    }


    if (ImGui::DragFloat("ScaleSpeed", (float*)&m_LocalData.fScaleSpeed))
    {
        m_pEffectData_Manager->Update_Data(m_DataName, m_LocalData);
    }


    ///////////
    if (ImGui::Checkbox("bUseRotationAnim", (bool*)&m_LocalData.bUseRotationAnim))
    {
        m_pEffectData_Manager->Update_Data(m_DataName, m_LocalData);

    }

    if (ImGui::DragFloat4("RotationAxis", (float*)&m_LocalData.vRotationAxis))
    {
        m_pEffectData_Manager->Update_Data(m_DataName, m_LocalData);
    }


    if (ImGui::DragFloat("RotationSpeed", (float*)&m_LocalData.fRotationSpeed))
    {
        m_pEffectData_Manager->Update_Data(m_DataName, m_LocalData);
    }
    //////////////
    if (ImGui::Checkbox("bUseMoveAnim", (bool*)&m_LocalData.bUseMoveAnim))
    {
        m_pEffectData_Manager->Update_Data(m_DataName, m_LocalData);

    }

    if (ImGui::DragFloat4("MoveDir", (float*)&m_LocalData.vMoveDir))
    {
        m_pEffectData_Manager->Update_Data(m_DataName, m_LocalData);
    }


    if (ImGui::DragFloat("MoveSpeed", (float*)&m_LocalData.fMoveSpeed))
    {
        m_pEffectData_Manager->Update_Data(m_DataName, m_LocalData);
    }
    /////////////
    if (ImGui::DragFloat("LifeTime", (float*)&m_LocalData.fLifeTime))
    {
        m_pEffectData_Manager->Update_Data(m_DataName, m_LocalData);

    }



    if (ImGui::DragFloat("Speed", (float*)&m_LocalData.fSpeed))
    {
        m_pEffectData_Manager->Update_Data(m_DataName, m_LocalData);

    }

    if (ImGui::Checkbox("Loop", (bool*)&m_LocalData.m_bLoop))
    {
        m_pEffectData_Manager->Update_Data(m_DataName, m_LocalData);

    }

    if (ImGui::Button("Play"))
        Play();


    if (ImGui::Button("Stop"))
        Stop();


    if (ImGui::Button("Save"))
    {
        m_pEffectData_Manager->Save_To_Json(m_DataName, m_LocalData);


    }
}
void CEffect::Update_Render(_float fTimeDelta)
{
	__super::Update_Render(fTimeDelta);
	m_pGameInstance->Add_RenderObject(m_eRenderGroup, this);
}

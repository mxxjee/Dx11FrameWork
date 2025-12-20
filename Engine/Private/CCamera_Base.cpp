#include "CCamera_Base.h"
#include "CGameInstance.h"
#include "CCameraComponent.h"
#include "CShader.h"


CCamera_Base::CCamera_Base(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
    :CGameObject(pDevice,pContext)
{

}

CCamera_Base::CCamera_Base(const CCamera_Base& rhs)
    :CGameObject(rhs),
   m_tRenderTarget(rhs.m_tRenderTarget),
    m_RenderMask(rhs.m_RenderMask)
{
}

HRESULT CCamera_Base::Initialize_Prototype()
{
  
    return S_OK;
}

HRESULT CCamera_Base::Initialize_Copytype(void* pArg)
{
    CAMERABASE_DESC* pDesc = static_cast<CAMERABASE_DESC*>(pArg);

    if(FAILED(__super::Initialize_Copytype(pArg)))
        return E_FAIL;

    /*DEsc값을 이용하여 멤버변수 채우기 ,렌더타겟뷰 생성*/
    if (FAILED(Ready_Resource(pArg)))
        return E_FAIL;

    m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&pDesc->vPosition), 1.f));
    m_pTransformCom->LookAt(XMVectorSetW(XMLoadFloat3(&pDesc->vAt), 1.f));
    
    return S_OK;
}

void CCamera_Base::Update_Late(_float fTimeDelta)
{
    Make_Planes();
}


void CCamera_Base::Update_PipeLine()
{
    //pipeline에게 정보업데이트
    m_pGameInstance->Set_Transform(ENUM_TO_UINT(m_eCameraType), D3DTS::VIEW, m_pTransformCom->Get_WorldInverse(TransformScope::WORLD));
    
    m_pGameInstance->Set_CamPosition(ENUM_TO_UINT(m_eCameraType), Get_CameraPosition());

    if (m_bPerspective)
        m_pGameInstance->Set_Transform(ENUM_TO_UINT(m_eCameraType), D3DTS::PROJ, XMMatrixPerspectiveFovLH(XMConvertToRadians(m_fFovy), (m_fWidth/m_fHeight), m_fNearZ, m_fFarZ));

    else
        m_pGameInstance->Set_Transform(ENUM_TO_UINT(m_eCameraType), D3DTS::PROJ, XMMatrixOrthographicLH(m_fWidth, m_fHeight, m_fNearZ, m_fFarZ));

}

HRESULT CCamera_Base::Create_RenderTagetview(bool bCreateRenderTarget)
{
    if (!bCreateRenderTarget)
    {
        //기존의 렌더타겟으로 세팅하기.
        m_tRenderTarget.RTV = m_pGameInstance->Get_BackBuffer_RTV();
        m_tRenderTarget.DSV = m_pGameInstance->Get_BackBuffer_DSV();


        //기존의 렌더타겟에서  colorTex가져오기
        ComPtr<ID3D11Resource> pResource = nullptr;
        m_tRenderTarget.RTV->GetResource(pResource.GetAddressOf());
        
        if(FAILED(pResource.As(&m_tRenderTarget.pColorTex)))
            return E_FAIL;



        //셰이더리소스뷰 만들기
        ComPtr<ID3D11Texture2D> pBackBuffer;
        if(FAILED(m_pGameInstance->Get_Buffer(&pBackBuffer, 0)))
            return E_FAIL;

        D3D11_TEXTURE2D_DESC desc;
        pBackBuffer->GetDesc(&desc);
        desc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
        desc.Usage = D3D11_USAGE_DEFAULT;

        ComPtr<ID3D11Texture2D> copyTex;
        m_pDevice->CreateTexture2D(&desc, nullptr, &copyTex);
        m_pContext->CopyResource(copyTex.Get(), pBackBuffer.Get());


        if(FAILED(m_pDevice->CreateShaderResourceView(copyTex.Get(), nullptr, m_tRenderTarget.SRV.GetAddressOf())))
            return E_FAIL;


    }

    return S_OK;
}

HRESULT CCamera_Base::Bind_RenderTarget()
{

    m_pContext->OMSetRenderTargets(0, nullptr, nullptr);

    //카메라 렌더타겟 바인딩
    ID3D11ShaderResourceView* nullSRV[1] = {};
    m_pContext->PSSetShaderResources(0, 1, nullSRV);

    m_pContext->OMSetRenderTargets(1, m_tRenderTarget.RTV.GetAddressOf(), m_tRenderTarget.DSV.Get());


    return S_OK;
}

HRESULT CCamera_Base::UnBind_RenderTarget()
{


    if (!m_bUseNewRenderTarget) 
        return S_OK;

    
    ID3D11RenderTargetView* nullRTV[1] = { nullptr };
    m_pContext->OMSetRenderTargets(1, nullRTV, nullptr);
    m_pContext->OMSetRenderTargets(0, nullptr, nullptr);
    
    return S_OK;
}




HRESULT CCamera_Base::Clear_RenderTargetView(const _float4* pClearColor)
{
   
    if (!m_bUseNewRenderTarget)
        return S_OK; 

    m_pContext->ClearDepthStencilView(m_tRenderTarget.DSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    m_pContext->ClearRenderTargetView(m_tRenderTarget.RTV.Get(), reinterpret_cast<const _float*>(pClearColor));
   

    return S_OK;
}

void CCamera_Base::Set_RenderAllRenderMask(bool bRender)
{
    for (int i = 0; i < m_RenderGroupMax; ++i)
        m_RenderMask[i] = bRender;

}



HRESULT CCamera_Base::Ready_Resource(void* pArg)
{
    m_RenderGroupMax = m_pGameInstance->Get_RenderGroupCount();
    m_RenderMask.resize(m_RenderGroupMax);

    for (int i = 0; i < m_RenderGroupMax; ++i)
        m_RenderMask[i] = true;


    CAMERABASE_DESC* pDesc = static_cast<CAMERABASE_DESC*>(pArg);

    m_eCameraType = pDesc->eCameraType;
    m_eCameraFlag = pDesc->eCameraFlag;

    m_fFovy = pDesc->fFovy;

    m_fWidth = pDesc->fWidth;
    m_fHeight = pDesc->fHeight;

    m_fNearZ = pDesc->fNear;
    m_fFarZ = pDesc->fFar;
    m_vOffset = pDesc->vOffset;

    m_vInitOffset = m_vOffset;


    m_bUseNewRenderTarget = pDesc->m_bCreateNewRenderTarget;

    if (FAILED(Create_RenderTagetview(pDesc->m_bCreateNewRenderTarget)))
        return E_FAIL;

    return S_OK;    
}

_float4 CCamera_Base::Get_CameraPosition()
{
    _float4 vPos;
    XMStoreFloat4(&vPos, m_pTransformCom->Get_State(STATE::POSITION));

    return vPos;
}

void CCamera_Base::Make_Planes()
{
    _float4x4 ViewProj;
    XMStoreFloat4x4(&ViewProj, m_pGameInstance->Get_ViewProjMatrix(ENUM_TO_UINT(m_eCameraType)));
    CheckTrue(m_eCameraType == CAMERA_TYPE::MINIMAP);

    m_Planes[ENUM_TO_UINT(PLANE::LEFT)].x = ViewProj._14 + ViewProj._11;
    m_Planes[ENUM_TO_UINT(PLANE::LEFT)].y = ViewProj._24 + ViewProj._21;
    m_Planes[ENUM_TO_UINT(PLANE::LEFT)].z = ViewProj._34 + ViewProj._31;
    m_Planes[ENUM_TO_UINT(PLANE::LEFT)].w = ViewProj._44 + ViewProj._41;



    m_Planes[ENUM_TO_UINT(PLANE::RIGHT)].x = ViewProj._14 - ViewProj._11;
    m_Planes[ENUM_TO_UINT(PLANE::RIGHT)].y = ViewProj._24 - ViewProj._21;
    m_Planes[ENUM_TO_UINT(PLANE::RIGHT)].z = ViewProj._34 - ViewProj._31;
    m_Planes[ENUM_TO_UINT(PLANE::RIGHT)].w = ViewProj._44 - ViewProj._41;

    m_Planes[ENUM_TO_UINT(PLANE::BOTTOM)].x = ViewProj._14 + ViewProj._12;
    m_Planes[ENUM_TO_UINT(PLANE::BOTTOM)].y = ViewProj._24 + ViewProj._22;
    m_Planes[ENUM_TO_UINT(PLANE::BOTTOM)].z = ViewProj._34 + ViewProj._32;
    m_Planes[ENUM_TO_UINT(PLANE::BOTTOM)].w = ViewProj._44 + ViewProj._42;

    m_Planes[ENUM_TO_UINT(PLANE::TOP)].x = ViewProj._14 - ViewProj._12;
    m_Planes[ENUM_TO_UINT(PLANE::TOP)].y = ViewProj._24 - ViewProj._22;
    m_Planes[ENUM_TO_UINT(PLANE::TOP)].z = ViewProj._34 - ViewProj._32;
    m_Planes[ENUM_TO_UINT(PLANE::TOP)].w = ViewProj._44 - ViewProj._42;


    m_Planes[ENUM_TO_UINT(PLANE::PLANE_NEAR)].x = ViewProj._13;
    m_Planes[ENUM_TO_UINT(PLANE::PLANE_NEAR)].y = ViewProj._23;
    m_Planes[ENUM_TO_UINT(PLANE::PLANE_NEAR)].z = ViewProj._33;
    m_Planes[ENUM_TO_UINT(PLANE::PLANE_NEAR)].w = ViewProj._43;



    m_Planes[ENUM_TO_UINT(PLANE::PLANE_FAR)].x = ViewProj._14 - ViewProj._13;
    m_Planes[ENUM_TO_UINT(PLANE::PLANE_FAR)].y = ViewProj._24 - ViewProj._23;
    m_Planes[ENUM_TO_UINT(PLANE::PLANE_FAR)].z = ViewProj._34 - ViewProj._33;
    m_Planes[ENUM_TO_UINT(PLANE::PLANE_FAR)].w = ViewProj._44 - ViewProj._43;

    //////Normals//////
    m_PlaneNormal[ENUM_TO_UINT(PLANE::LEFT)] =
        _float3(
            m_Planes[ENUM_TO_UINT(PLANE::LEFT)].x
            , m_Planes[ENUM_TO_UINT(PLANE::LEFT)].y
            , m_Planes[ENUM_TO_UINT(PLANE::LEFT)].z);

    m_PlaneNormal[ENUM_TO_UINT(PLANE::RIGHT)] =
        _float3(
            m_Planes[ENUM_TO_UINT(PLANE::RIGHT)].x
            , m_Planes[ENUM_TO_UINT(PLANE::RIGHT)].y
            , m_Planes[ENUM_TO_UINT(PLANE::RIGHT)].z);

    m_PlaneNormal[ENUM_TO_UINT(PLANE::BOTTOM)] =
        _float3(
            m_Planes[ENUM_TO_UINT(PLANE::BOTTOM)].x
            , m_Planes[ENUM_TO_UINT(PLANE::BOTTOM)].y
            , m_Planes[ENUM_TO_UINT(PLANE::BOTTOM)].z);

    m_PlaneNormal[ENUM_TO_UINT(PLANE::TOP)] =
        _float3(
            m_Planes[ENUM_TO_UINT(PLANE::TOP)].x
            , m_Planes[ENUM_TO_UINT(PLANE::TOP)].y
            , m_Planes[ENUM_TO_UINT(PLANE::TOP)].z);

    m_PlaneNormal[ENUM_TO_UINT(PLANE::PLANE_FAR)] =
        _float3(
            m_Planes[ENUM_TO_UINT(PLANE::PLANE_FAR)].x
            , m_Planes[ENUM_TO_UINT(PLANE::PLANE_FAR)].y
            , m_Planes[ENUM_TO_UINT(PLANE::PLANE_FAR)].z);


    m_PlaneNormal[ENUM_TO_UINT(PLANE::PLANE_NEAR)] =
        _float3(
            m_Planes[ENUM_TO_UINT(PLANE::PLANE_NEAR)].x
            , m_Planes[ENUM_TO_UINT(PLANE::PLANE_NEAR)].y
            , m_Planes[ENUM_TO_UINT(PLANE::PLANE_NEAR)].z);

    //모두정규화
    //법선,D 정규화
    for (int i = 0; i < ENUM_TO_UINT(PLANE::END); ++i)
    {
        
        _float PlaneNormalLegth= XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_PlaneNormal[i])));

        m_PlaneNormal[i].x = m_PlaneNormal[i].x / PlaneNormalLegth;
        m_PlaneNormal[i].y = m_PlaneNormal[i].y / PlaneNormalLegth;
        m_PlaneNormal[i].z = m_PlaneNormal[i].z / PlaneNormalLegth;
        

        m_Planes[i].w /= PlaneNormalLegth;


    }
 
   
}
bool CCamera_Base::IsInDistance(const _float3 vCenter)
{
    _vector campos = m_pTransformCom->Get_State(STATE::POSITION);

    
    float dist = XMVectorGetX(XMVector3Length(campos - XMLoadFloat3(&vCenter)));
    if (dist > m_fCulDistance)
        return false;

    return true;
}
bool CCamera_Base::IsInFrustum(const _float3& MinBound, const _float3& MaxBound)
{
    for (int i = 0; i < ENUM_TO_UINT(PLANE::END); ++i)
    {
        //하나라도밖에있으면 렌더하지않는다.
        
        if (IsOutSidePlane((_uint)i, MinBound, MaxBound))
            return false;
    }
    return true;
}
bool CCamera_Base::IsOutSidePlane(_uint PlaneDir, const _float3& MinBound, const _float3& MaxBound)
{
    _float3       PositiveVertex;

    PositiveVertex.x = (m_PlaneNormal[PlaneDir].x >= 0) ? MaxBound.x : MinBound.x;
    PositiveVertex.y = (m_PlaneNormal[PlaneDir].y >= 0) ? MaxBound.y : MinBound.y;
    PositiveVertex.z = (m_PlaneNormal[PlaneDir].z >= 0) ? MaxBound.z : MinBound.z;

    float distance = m_PlaneNormal[PlaneDir].x * PositiveVertex.x +
        m_PlaneNormal[PlaneDir].y * PositiveVertex.y +
        m_PlaneNormal[PlaneDir].z * PositiveVertex.z+
        m_Planes[PlaneDir].w;

    //양수라면, 평면바깥에존재
    return distance<0.0f;
}
void CCamera_Base::Free()
{
    __super::Free();
    
    m_tRenderTarget.SRV.Reset();
    m_tRenderTarget.pColorTex.Reset();
    m_tRenderTarget.DSV.Reset();
  
    m_tRenderTarget.RTV.Reset();



}

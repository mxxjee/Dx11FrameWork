#include "CBackGround.h"
#include "CGameInstance.h"

#include "CVertexBuffer.h"
#include "CIndexBuffer.h"
#include "CInputLayout.h" 
#include "CGeometry.h"
#include "CVertexShader.h"
#include "CPixelShader.h"
#include "GeometryHelper.h"
#include "CDefaultShader.h"
#include "CConstantBuffer.h"
#include "CTexture.h"
#include "Client_Defines.h"


USING(Client)

CBackGround::CBackGround(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
    :CPanel(_pDevice, _pDeviceContext)
{
}

CBackGround::CBackGround(const CBackGround& rhs)
    : CPanel(rhs)
{
}

HRESULT CBackGround::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CBackGround::Initialize_Copytype(void* pArg)
{
    /*값세팅 */

    
    if (FAILED(__super::Initialize_Copytype(pArg)))
        return E_FAIL;

    if (FAILED(Add_Component(pArg)))
        return E_FAIL;

 

    return S_OK;
}

void CBackGround::Update_Priority(_float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
}

void CBackGround::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
  
 

}

void CBackGround::Update_Late(_float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);
    if (GetKeyState(VK_RIGHT) & 0x8000)
        m_pTransformCom->Move(DIRECTION::RIGHT, fTimeDelta);


    else if (GetKeyState(VK_LEFT) & 0x8000)
        m_pTransformCom->Move(DIRECTION::LEFT, fTimeDelta);

    else if (GetKeyState(VK_UP) & 0x8000)
        m_pTransformCom->Move(DIRECTION::UP, fTimeDelta);

    else if (GetKeyState(VK_DOWN) & 0x800)
        m_pTransformCom->Move(DIRECTION::DOWN, fTimeDelta);

 
    //m_pTransformCom->LookAt(
    //    m_pTransformCom->Get_State(STATE::UP),
    //    XMVectorSet(100.f, 0.f, 0.f, 1.f), 
    //    fTimeDelta);


}

void CBackGround::Update_Render(_float fTimeDelta)
{
    __super::Update_Render(fTimeDelta);
  
}

HRESULT CBackGround::Render()
{
    
    __super::Render();

    return S_OK;
}


HRESULT CBackGround::Add_Component(void* pArg)
{
    //컴포넌트생성
    UI_DESC* pDesc = static_cast<UI_DESC*>(pArg);
    pDesc->pOwner = this;

 

    return S_OK;
}


CBackGround* CBackGround::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext)
{

    CBackGround* pInstance = new CBackGround(_pDevice, _pDeviceContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Create :CBackGround ");
        Safe_Release(pInstance);

    }



    return pInstance;
}

CGameObject* CBackGround::Clone(void* pArg)
{
    CBackGround* pInstance = new CBackGround(*this);
    if (FAILED(pInstance->Initialize_Copytype(pArg)))
    {
        MSG_BOX("Failed to Cloned :CBackGround ");
        Safe_Release(pInstance);

    }
    return pInstance;
}

void CBackGround::Free()
{
    __super::Free();

}

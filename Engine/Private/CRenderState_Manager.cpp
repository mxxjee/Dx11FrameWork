#include "CRenderState_Manager.h"

CRenderState_Manager::CRenderState_Manager(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pContext)
    :m_pDevice(_pDevice), m_pContext(_pContext)
{
}

HRESULT CRenderState_Manager::Initialize()
{
    
    return S_OK;
}

HRESULT CRenderState_Manager::Register_RenderStates(_uint iRenderGroup, const RenderStates& States)
{
    auto iter = m_RenderStates.find(iRenderGroup);
    if (iter != m_RenderStates.end())
        return E_FAIL;

    m_RenderStates.emplace(iRenderGroup, States);

}

const RenderStates& CRenderState_Manager::Get_RenderStates(_uint iRenderGroup)
{
    RenderStates RenderState;
    auto iter = m_RenderStates.find(iRenderGroup);
    if (iter != m_RenderStates.end())
        return iter->second;

    return RenderState;


}

CRenderState_Manager* CRenderState_Manager::Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pContext)
{
    CRenderState_Manager* pInstance = new CRenderState_Manager(_pDevice, _pContext);
    if(FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Create : CRenderState_Manager");
        Safe_Release(pInstance);
    }

    return pInstance;
}

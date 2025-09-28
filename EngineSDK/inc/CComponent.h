#pragma once
#include "CBase.h"

NS_BEGIN(Engine)

class ENGINE_DLL CComponent :
    public CBase
{
public:
    typedef struct tagComponentDesc
    {
        class CGameObject* pOwner = nullptr;
        void* TransformDesc = nullptr;
    
    
    }COMPONENT_DESC;

protected:
    CComponent(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CComponent(const CComponent& Prototype);
    virtual ~CComponent() = default;



public:
    virtual         HRESULT     Initialize_Prototype();
    virtual         HRESULT     Initialize_Copytype(void* pArg);



protected:
    ComPtr<ID3D11Device> m_pDevice = nullptr;
    ComPtr<ID3D11DeviceContext> m_pContext = nullptr;
    class CGameInstance* m_pGameInstance = nullptr;

public:
    virtual CComponent* Clone(void* pArg) = 0;
    virtual void Free() override;

protected:
    class CGameObject* m_pOwner = nullptr;
};
NS_END

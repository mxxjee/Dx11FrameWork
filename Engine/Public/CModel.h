#pragma once
#include "CComponent.h"
#include "ModelData.h"

NS_BEGIN(Engine)

class CMeshComponent;
class CGameInstance;


class ENGINE_DLL CModel :
    public CComponent
{
protected:
    CModel(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CModel(const CModel& Prototype);
    virtual ~CModel() = default;

public:
    virtual HRESULT Initialize_Prototype(const _char* pFilePath);
    virtual HRESULT Initialize_Copytype(void* pArg) override;



private:
    HRESULT     LoadModelFromJson(const _char* filepPath);
    HRESULT     LoadMaterialFromJSon(const _char* filePath);


public:

    static CModel* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext,const _char* pFilePath);
    virtual CComponent* Clone(void* pArg) override;
    virtual void Free() override;


    HRESULT                 Render();


private:
    UMap<wstring, CMeshComponent*>      m_Meshs;
    ModelData       m_ModelData;


private:
    CGameInstance* m_pGameInstance = nullptr;
};
NS_END

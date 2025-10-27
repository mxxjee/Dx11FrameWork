#pragma once
#include "CComponent.h"

NS_BEGIN(Engine)
class ENGINE_DLL CModel :
    public CComponent
{
protected:
    CModel(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CModel(const CModel& Prototype);
    virtual ~CModel() = default;

public:
    virtual HRESULT Initialize_Prototype(const _char* pModelFilePath);
    virtual HRESULT Initialize_Copytype(void* pArg) override;

public:
    static CModel* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, const _char* pModelFilePath);
    virtual CComponent* Clone(void* pArg) override;
    virtual void Free() override;


private:
    Assimp::Importer        m_Importer = {};        //모델 로드하기 위한 임포터
    const aiScene*          m_pAIScene = { nullptr };            //읽어온 모델의 정보를 aiscene 구조체에 저장하는형태
};
NS_END

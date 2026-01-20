#pragma once
#include "CBase.h"

/*모델 원본의 저장소..*/
/*모델 원본을 찾아서 사본을 리턴해준다,,*/
NS_BEGIN(Engine)
class CModel;

class CModel_Manager :
    public CBase
{
private:
    CModel_Manager(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pContext);
    virtual ~CModel_Manager() = default;

public:
    HRESULT        Initialize();
    HRESULT       Register_Model(const _wstring& Tag, CModel* pInstance);

public:
    static CModel_Manager* Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pContext);
    virtual void                Free() override;

public:
    _uint           Get_ModelCount() {return m_iCount;}
    const UMap<_wstring, CModel*>& Get_MapModel() { return m_mapModel; }
public:
        //원본을 찾는다.
    CModel* Find_Model(const _wstring& ProtoModelName);
    CModel* Clone_Model(const _wstring& ProtoModelName, void* pArg);

public:
    //모든 모델 로드..(폴더안의 모델 모두로드)
    HRESULT Load_All_Models(const string& FilePath, _matrix PreMatrix);

    //특정 모델 로드..
    HRESULT Load_Model(const string& FilePath, _matrix PreMatrix);

        //특정 경로의 모델 로드

private:
    UMap<_wstring, CModel*>    m_mapModel;
    _uint                   m_iCount=0;
private:
    ComPtr<ID3D11Device> m_pDevice = { nullptr };
    ComPtr<ID3D11DeviceContext> m_pContext = { nullptr };


};
NS_END


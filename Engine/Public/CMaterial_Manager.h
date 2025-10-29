#pragma once
#include "CBase.h"


NS_BEGIN(Engine)
class CMaterial;


class CMaterial_Manager :
    public CBase
{
private:
    CMaterial_Manager(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pContext);
    virtual ~CMaterial_Manager() = default;
public:
    HRESULT        Initialize();
    HRESULT       Register_Material(const _wstring & Tag, CMaterial* pInstance);

public:
    static CMaterial_Manager* Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pContext);
    virtual void                Free() override;

public:
    CMaterial* Find_Material(const _wstring & Tag);

private:
    UMap<_wstring, CMaterial*>    m_mapMat;

private:
    ComPtr<ID3D11Device> m_pDevice = { nullptr };
    ComPtr<ID3D11DeviceContext> m_pContext = { nullptr };
};
NS_END


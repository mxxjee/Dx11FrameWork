#pragma once
#include "CPanel.h"

/*뒤에 흑백이미지, 흑백 pass사용*/
NS_BEGIN(Client)
class CLoadingUI :
    public CPanel
{
protected:
    CLoadingUI(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext);
    CLoadingUI(const CLoadingUI& rhs);
    virtual ~CLoadingUI() = default;
   
public:
    virtual HRESULT Initialize_Prototype() override; /* 원형객체가 만들어질 때 호출되는 함수. 무거운 초기화작업.  */
    virtual HRESULT Initialize_Copytype(void* pArg) override; /* 원형을 복제하여 만든 사본객체가 생성시에 호출되는 함수. 추가적인 초기화를 위해 */

    virtual void Update_Priority(_float fTimeDelta) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Update_Late(_float fTimeDelta) override;
    virtual void Update_Render(_float fTimeDelta) override;
    virtual HRESULT Render() override;

public:
    void        Set_Progress(_float fValue) { m_fProgress = _float4(fValue,fValue,fValue,fValue); }
public:
    static CPanel* Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;

private:
    _float4      m_fProgress = _float4(0.f,0.f,0.f,0.f);

};
NS_END


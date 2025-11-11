#pragma once


/*ContainerObject가 포함할 수 있는 오브젝트들.
어딘가에 부착해서 사용하므로 부모행렬을 항상갖는다.
나 자신의 로컬 SRT  * 부모 SRT = 위치 */

#include "CGameObject.h"

NS_BEGIN(Engine)
class ENGINE_DLL CPartObject :
    public CGameObject
{
public:
    typedef struct tagPartObjectDesc
    {
        const _float4x4* pParentMatrix = { nullptr };//포인터참조
    }PARTOBJECT_DESC;
protected:
    CPartObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CPartObject(const CPartObject& Prototype);
    virtual ~CPartObject() = default;


public:
    virtual HRESULT     Initialize_Prototype(); /*원형 객체가 생성될때 부르는 Initialize*/
    virtual HRESULT     Initialize_Copytype(void* pArg); /*사본 객체가 생성될때 부르는 Initialize*/

    virtual void        Update_Priority(_float fTimeDelta);
    virtual void        Update(_float fTimeDelta);
    virtual void        Update_Late(_float fTimeDelta);
    virtual void        Update_Render(_float fTimeDelta);


    virtual HRESULT Render();



protected:
    const _float4x4* m_pParentMatrix = { nullptr };
    _float4x4       m_CombinedWorldMatrix = {};     //로컬 * parent matrix

protected:
    //m_combineWorldMAtrix를 도출하는함수
    void    SetUp_CombinedWorldMatrix(_fmatrix ParentMatrix); 

public:
    virtual CGameObject* Clone(void* pArg) = 0;
    virtual void    Free() override;
};

NS_END


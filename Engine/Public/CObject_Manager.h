#pragma once
#include "CBase.h"

/* 게임 내에서 실제 사용을 위한 객체들을 복제하여 생성한 후, 보관한다. */
/* 개발자가 원하는대로 묶어서(CLayer) 레벨별로([]) 구분하여 보관한다. */
/* 레벨이 이용하는 레이어들을 반복적으로 갱신해 준다.  */
/* 렌더까지는 수행하지 않는다.(CRenderer) */

NS_BEGIN(Engine)

class CLayer;
class CGameInstance;

class CObject_Manager final :
    public CBase
{
private:
    CObject_Manager(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    virtual ~CObject_Manager() = default;

public:
    HRESULT Initialize(_uint iNumLevels);
    void Update_Priority(_float fTimeDelta);
    void Update(_float fTimeDelta);
    void Update_Late(_float fTimeDelta);
    void Update_Render(_float fTimeDelta);


public:
    //생성과 동시에 추가해주는 함수
    HRESULT     Add_GameObject_To_Layer(_uint iProtoLevelIndex, const _wstring& strPrototypeTag,
        _uint iLayerLevelIndex, const _wstring& strLayerTag, void* pArg = nullptr);

    //이미생성한 것을 추가하는 함수
    HRESULT     Add_GameObject_To_Layer(_uint iLayerLevelIndex, const _wstring& strLayerTag,CGameObject* pObject);


    const unordered_map<_wstring, CLayer*>& Get_Layers(_uint iLevel);
    class CGameObject* Find_GameObject(_uint iLevelIndex, const _wstring& LayerTag, const _wstring& Tag);
    void    Clear(_uint iLevelIndex);

public:
    void        Update_Priority_Static(_float fTimeDelta);
    void        Update_Static(_float fTimeDelta);
    void        Update_Late_Static(_float fTimeDelta);
    void        Update_Render_Static(_float fTimeDelta);
private:
    CLayer* Find_Layer(_uint iLevelIndex, const _wstring& LayerTag);
    

private:
    ComPtr<ID3D11Device> m_pDevice;
    ComPtr<ID3D11DeviceContext> m_pContext;
    CGameInstance* m_pGameInstance      = { nullptr };


private:
    vector<unordered_map<_wstring, CLayer*>> m_Layers;
    _uint                    m_iNumLevels = {};/*최대 레벨 개수저장*/

public:
    static CObject_Manager* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, _uint iNumLevels);
    virtual void Free();
};
NS_END


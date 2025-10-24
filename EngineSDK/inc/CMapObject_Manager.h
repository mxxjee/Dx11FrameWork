#pragma once
#include "CBase.h"


NS_BEGIN(Engine)

class CMapObject;
class CLayer;
class CGameInstance;

class CMapObject_Manager :
    public CBase
{
private:
    CMapObject_Manager(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pContext);
    virtual ~CMapObject_Manager() = default;


public:
    void Update_Priority(_float fTimeDelta);
    void Update(_float fTimeDelta);
    void Update_Late(_float fTimeDelta);
    void Update_Render(_float fTimeDelta);

    void    Check_Picking();

public:
    //생성과 동시에 추가해주는 함수
    HRESULT             Add_MapObject_To_Layer(_uint iProtoLevelIndex, const _wstring& strPrototypeTag,const _wstring& strLayerTag, void* pArg = nullptr);

    //이미생성한 것을 추가하는 함수
    HRESULT                 Add_MapObject_To_Layer(const _wstring& LayerTag,CMapObject* pObj);
    CMapObject*             Find_MapObject(const _wstring& LayerTag,const _wstring& ObjTag);
    void                    Clear(const _wstring& LayerTag);
    CLayer* Find_Layer(const _wstring& LayerTag);
    void            Set_SelectObject(CMapObject* pObj);


public:
    //레이어리스트를 가져온다.
    const UMap<_wstring, CLayer*>&    Get_Layers() { return m_Layers; }
    CMapObject* Get_SelectObject() { return m_pSelectObject; }

public:
   
public:
    UMap<_wstring, CLayer*> m_Layers;

private:
    ComPtr<ID3D11Device> m_pDevice;
    ComPtr<ID3D11DeviceContext> m_pContext;
    CGameInstance* m_pGameInstance = { nullptr };


public:
    static CMapObject_Manager* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    virtual void Free();

private:
    CMapObject*         m_pSelectObject = nullptr;
    ENGINE_DESC             m_EngineDesc;

};


NS_END


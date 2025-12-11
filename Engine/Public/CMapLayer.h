#pragma once
#include "CBase.h"

NS_BEGIN(Engine)

class CMapObject;

class ENGINE_DLL CMapLayer :
    public CBase
{
private:
    CMapLayer();
    virtual ~CMapLayer() = default;

public:
    HRESULT     Add_GameObject(CMapObject* pGameObject);
    void Update_Priority(_float fTimeDelta);
    void Update(_float fTimeDelta);
    void Update_Late(_float fTimeDelta);
    void Update_Render(_float fTimeDelta);

    const list<CMapObject*>& Get_ObjList() { return m_ObjList; }
    CMapObject* Find_GameObject(const _wstring& Tag);
    CMapObject* Find_GameObject(_uint iIdx);

   
    CMapObject*        Check_Picking(_float3& vWorld,HWND g_hWnd, ComPtr<ID3D11DeviceContext> Context, _float4x4& Proj, _float4x4& View,float& Dist);

public:
    void    RequestDestroy(CMapObject* pObj);//DestroyQue에 추가하라는 작업

    void    ProcessDestroy();       //DestroyQue를 비우는작업
    
public:
    void        Set_AblePicking(bool b) { m_bAblePicking = b; }
    HRESULT     Save_Data(const string& filePath, _uint iNum);
    HRESULT     Load_Data(const string& LoadPath,vector<DefaultInteractionData>& Infos);
private:
    list<CMapObject*>      m_ObjList;
    queue<CMapObject*>      m_DestroyQueue;


public:
    static CMapLayer* Create();
    virtual void Free() override;

private:
    bool            m_bAblePicking = true;
};

NS_END
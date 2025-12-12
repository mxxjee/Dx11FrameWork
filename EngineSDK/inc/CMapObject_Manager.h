#pragma once
#include "CBase.h"
#include "CBounding_Mesh.h"
#include "Parsing_RoomInfo.h"

NS_BEGIN(Engine)

class CMapObject;
class CMapLayer;
class CGameInstance;
class IMapEditable;
class CInput_Manager;




class ENGINE_DLL CMapObject_Manager :
    public CBase
{

    DECLARE_SINGLETON(CMapObject_Manager)

private:
    explicit CMapObject_Manager();
    CMapObject_Manager(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pContext);
    virtual ~CMapObject_Manager() = default;


public:
    HRESULT     Initialize(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pContext);
    void Update_Priority(_float fTimeDelta);
    void Update(_float fTimeDelta);
    void Update_Late(_float fTimeDelta);
    void Update_Render(_float fTimeDelta);

    void    Check_Picking();


public:
    //생성과 동시에 추가해주는 함수
    HRESULT             Add_MapObject_To_MapLayer(_uint iProtoLevelIndex, const _wstring& strPrototypeTag,const _wstring& strLayerTag, void* pArg = nullptr);

    //이미생성한 것을 추가하는 함수
    HRESULT                 Add_MapObject_To_MapLayer(const _wstring& LayerTag,CMapObject* pObj);
    



    CMapObject*             Find_MapObject(const _wstring& LayerTag,const _wstring& ObjTag);
    void                    Clear(const _wstring& LayerTag);
    CMapLayer*              Find_MapLayer(const _wstring& LayerTag);
  
public:
                                    //interaction type에 맞는 meshcollider 값채우기
    CBounding_Mesh::BOUNDING_MESH_DESC           Generate_Collider_By_InteractionType(_uint i);

public:
    HRESULT             Save_Data(const wstring& Path);
    CMapLayer*             Get_Layer_By_MapObjType(MapObjType eType);
    
public:
    HRESULT         Save_InteractionData(const string& filePath, _uint iNum = 0);
    HRESULT         Load_InteractionData(const string& filePath, vector<DefaultInteractionData>& Datas);
         
 
    HRESULT         Save_RoomData(_uint iRoomNum = 1);
    HRESULT         Load_RoomData(const string& filePath,
                                            RoomInfo& Info);

public:
    wstring         Generate_UniqueTag(MapObjType Type, const wstring& baseName);
   public:
    //레이어리스트를 가져온다.
    const UMap<_wstring, CMapLayer*>&    Get_MapLayers() { return m_Layers; }

public:
    void        Show_SelectObject_Gizmo();

public:
    void                    Set_SelectObject(IMapEditable* pSelectedObject);
    IMapEditable*           Get_SelectObject() { return m_pSelectedObject; }
    UMap<_wstring, CMapLayer*> m_Layers;        //월드상에 배치된애들(지형제외)


public:
    _float      Get_MoveSpeed() { return m_fMoveSpeed; }
    void        Set_MoveSpeed(_float Speed) { m_fMoveSpeed = Speed; }

    void        Set_AblePicking(bool Picking) { m_bAblePicking = Picking; }
    void        Set_Dead(bool b) { m_bDestroy = true; }

private:
    ComPtr<ID3D11Device> m_pDevice;
    ComPtr<ID3D11DeviceContext> m_pContext;
    CGameInstance* m_pGameInstance = { nullptr };
    class CInput_Manager* m_pInputManager = { nullptr };
    class CImGui_Manager* m_pImguiManager = { nullptr };

public:
    static CMapObject_Manager* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    virtual void Free();


    ENGINE_DESC             m_EngineDesc;



protected:
    BoundingSphere      m_Sphere;
    IMapEditable* m_pSelectedObject = { nullptr };

public:
    _float3             Get_PickingWorldPos() { return PickingWolrdPos; }
private:
    float           m_fMoveSpeed = 5;
    bool            m_bAblePicking = true;
    bool            m_bDestroy = false;
    _float3         PickingWolrdPos;     //terrain픽킹한 월드좌표

};


NS_END


#pragma once

/*맵툴에서 설치할 FIELD메쉬,
클라에서 부를떄 생성할 객체타입*/


#include "CTerrain_Base.h"
#include "IMapEditable.h"

NS_BEGIN(Engine)
class CMeshColliderComponent;
class CModel;
class CInput_Manager;

class ENGINE_DLL CMapTerrain :
    public CTerrain_Base, public IMapEditable
{
public:
    typedef struct tagMapTerrain_DESC : CTerrain_Base::tagTerrainDesc
    {
        MapObjType              ObjType;
        _wstring    modelName;
        void* modelDesc = nullptr;

    }MAPTERRAIN_DESC;

protected:
    CMapTerrain(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CMapTerrain(const CMapTerrain& rhs);
    virtual ~CMapTerrain() = default;

public:
    virtual HRESULT     Initialize_Prototype(); /*원형 객체가 생성될때 부르는 Initialize*/
    virtual HRESULT     Initialize_Copytype(void* pArg); /*사본 객체가 생성될때 부르는 Initialize*/

    virtual void        Update_Priority(_float fTimeDelta);
    virtual void        Update(_float fTimeDelta);
    virtual void        Update_Late(_float fTimeDelta);
    virtual void        Update_Render(_float fTimeDelta) override;


    virtual HRESULT Render();

public:
    HRESULT                     Ready_Components(void* pArg);
    HRESULT                     Ready_Resources(void* pArg);
    HRESULT                     Bind_ShaderResources();

public:
    static CMapTerrain* Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;

public:
   // 픽킹 확인
    bool            Is_Picked(_vector Origin, _vector Dir, float& Dist);

    void            Set_CanPicking(bool bPicking) { m_bCanPicking = bPicking; }
    bool            Is_CanPicking() { return m_bCanPicking; }

    void            Set_Idx(int _idx) { m_iIdx = _idx; }
    int             Get_Idx() { return m_iIdx; }


public:
    // IMapEditable을(를) 통해 상속됨
    virtual void OnSeletected(bool bSelected) override;

    virtual void Save_To_Json(json& Json) override;
    virtual void Edit_Move(DIRECTION eDir, float fSpeed, float _fTimeDelta);
    virtual void Update_SelectMode(float _fTimeDelta);

protected:
    CModel* m_pModel = nullptr;
    CMeshColliderComponent* m_pMeshCollidercomponent=nullptr;

private:
    _uint                       m_iIdx = 0;
    bool                         m_bCanPicking = true;


    // IMapEditable을(를) 통해 상속됨
    virtual void Show_Gizmo() override;


    // IMapEditable을(를) 통해 상속됨
    virtual void Imgui_Render_Properties(_float3* vScale, _float3* vPosition, _float3* vRotation) override;
    virtual void Fix_Y(_float Y);

private:
    class CInput_Manager* m_pInputManager = nullptr;
};

NS_END


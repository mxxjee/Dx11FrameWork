#pragma once

#include "Engine_Define.h"
#include "CGameObject.h"
#include "IMapEditable.h"



/*기본생성 : transform , collider(픽킹),셰이더*/
NS_BEGIN(Engine)
class CCollider_Base;
class CMapObject_Manager;

class ENGINE_DLL CMapObject :
    public CGameObject,public IMapEditable
{
public:
    typedef struct MapObject_DESC : CGameObject::GAMEOBJECT_DESC
    {
        MapObjType              ObjType;
        _uint                   eRenderGroup = 0;
        void*                   ColliderComponent=nullptr;


        _wstring    ShaderName = L"Default";
        string      passName = "Default";

    }MapObject_DESC;

protected:
    CMapObject(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    CMapObject(const CMapObject& rhs);
    virtual ~CMapObject() = default;


public:
    HRESULT     Initialize_Prototype(); /*원형 객체가 생성될때 부르는 Initialize*/
    virtual HRESULT     Initialize_Copytype(void* pArg); /*사본 객체가 생성될때 부르는 Initialize*/

    void        Update_Priority(_float fTimeDelta);
    void        Update(_float fTimeDelta);
    void        Update_Late(_float fTimeDelta);
    void        Update_Render(_float fTimeDelta);

    virtual void        OnDestroy() {};

    virtual HRESULT Render();

public:
                //픽킹 확인
    virtual bool            Is_Picked(_vector Origin, _vector Dir, float& Dist);
    virtual void    Set_Active(bool _b);

public:
    virtual HRESULT     Ready_Component(void* pArg);
    HRESULT     Ready_Resource(void* pArg);


public:
    void        Set_Select(bool _bSelect) { m_bSelected = _bSelect; }
    virtual     void    Free() override;


protected:
    CCollider_Base* pColliderComp = { nullptr };
    class CShader* m_pShader = nullptr;

protected:
    _uint                   m_eRenderGroup = 0;
  
    _wstring                m_ShaderName = L"";
    string                  m_passName = "";


public:
    CMapObject_Manager*         m_pMapObject_Manager = nullptr;

    // IMapEditable을(를) 통해 상속됨
    virtual void OnSeletected(bool bSelected) override;
    virtual void Save_To_Json(json& Json) override;

    // IMapEditable을(를) 통해 상속됨
    virtual void Show_Gizmo() override;

    // CGameObject을(를) 통해 상속됨
    virtual CGameObject* Clone(void* pArg) override;


public:
    // IMapEditable을(를) 통해 상속됨
    virtual void Imgui_Render_Properties(_float3* vScale, _float3* vPosition, _float3* vRotation) override;
    virtual void Edit_Move(DIRECTION eDir, float fSpeed, float _fTimeDelta);
    virtual void Fix_Y(_float Y);
    virtual void Update_SelectMode(float _fTimeDelta);

private:
    class CInput_Manager* m_pInputManager = { nullptr };

protected:
    bool        m_bAblePicking = true;

};


NS_END


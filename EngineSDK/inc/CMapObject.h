#pragma once

#include "Engine_Define.h"
#include "CGameObject.h"
#include "IMapEditable.h"



/*기본생성 : transform , collider(픽킹),셰이더*/
NS_BEGIN(Engine)
class CBoxColliderComponent;
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


    virtual HRESULT Render();

public:
                //픽킹 확인
    bool            Is_Picked(_vector Origin, _vector Dir, float& Dist);

private:
    HRESULT     Ready_Component(void* pArg);
    HRESULT     Ready_Resource(void* pArg);


public:
    void        Set_Select(bool _bSelect) { m_bSelected = _bSelect; }
    virtual     void    Free() override;


protected:
    CBoxColliderComponent* pColliderComp = { nullptr };
    class CShader* m_pShader = nullptr;

protected:
    _uint                   m_eRenderGroup = 0;
  
    _wstring                m_ShaderName = L"";
    string                  m_passName = "";


private:
    CMapObject_Manager*         m_pMapObject_Manager = nullptr;

    // IMapEditable을(를) 통해 상속됨
    virtual void OnSeletected(bool bSelected) override;
    virtual void Save_To_Json(json& Json) override;
};


NS_END


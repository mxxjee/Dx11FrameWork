#pragma once
#include "CImgui_Window.h"
#include "Client_Defines.h"


namespace Engine
{
    class CGameObject;
    class CModelObject;
    class CAnimModelObject;

}

NS_BEGIN(Client)
class CPlayer;
class CStateDebugWindow :
    public CImgui_Window
{
protected:
    CStateDebugWindow(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    virtual ~CStateDebugWindow() = default;
public:
    // CImgui_Base을(를) 통해 상속됨
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Update() override;
    virtual void Render() override;

public:
    void        Set_Player(CPlayer* pPlayer) { m_pPlayer = pPlayer; }
    virtual         void    Reset() { pSelectObject = nullptr; }        //리셋하고싶은 값 리셋하기

public:
    void        Set_SelectObject(CGameObject* pObj);
public:
    static CStateDebugWindow* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg);
    virtual void Free();

private:
    CPlayer* m_pPlayer = nullptr;
    tagActionControl* pActionControl = nullptr;
    int Value = 0;

    CGameObject* pSelectObject = { nullptr };
    CModelObject* pModel = nullptr;
    CAnimModelObject* ppAnimModel = nullptr;
};
NS_END


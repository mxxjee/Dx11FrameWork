#pragma once
#include "CImgui_Window.h"

namespace Engine
{
    class CImgui_InputInt;
    class CImgui_InputFloat;
    class CImgui_Button;
    class CGameInstance;
    class CMapObject_Manager;
}
NS_BEGIN(MapTool)


class CTerrainBuildWindow :
    public CImgui_Window
{
    enum RANGE
    {
        MIN,MAX
    };
protected:
    CTerrainBuildWindow(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext);
    virtual ~CTerrainBuildWindow() = default;

private:
    HRESULT     Create_Widgets();
    HRESULT        Create_Terrain(const wstring& Tag,_float3 vPosition);
public:
    // CImgui_Base을(를) 통해 상속됨
    virtual HRESULT Initialize(void* pArg) override;
    virtual void Update() override;
    virtual void Render() override;
public:
    static CTerrainBuildWindow* Create(ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext, void* pArg);
    virtual void Free();



private:
    CImgui_InputInt* ModelNumInput[2] = { nullptr };        //몇번모델번호부터사용할거지? EX) (10a) = 10
    CImgui_InputFloat* OffSet[2] = { nullptr };        //몇번모델번호부터사용할거지? EX) (10a) = 10
    CImgui_InputFloat* buildPosition[3]= { nullptr };      //build 시작좌표, 보통 그리드의 좌상단

    CImgui_Button* m_pBuildButton=nullptr;          //설치시작

private:
    void            Start_Build();

private:
    int             m_iStartModelNum = 1;
    int             m_iEndModelNum = 16;

    _float3             m_fStartPos = { 0.f,0.f,100.f };        //설치를 시작할 점(보통 그리드의 좌상단)
    _float3             m_fOffSet = { 15.f,0.f,12.f };

    CGameInstance*      m_pGameInstance = nullptr;
    CMapObject_Manager* m_pMapObjectManager = nullptr;


};
NS_END

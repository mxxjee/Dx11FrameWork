#pragma once
#include "CBase.h"



/*엔진의 모든 매니저들을 관리한다.
1. 엔진의 모든 매니저들을 업데이트/렌더한다.
2. 여러 매니저들의 함수를 호출해주는 역할 (링크)
3. 알아서 Update한다.
4. 알아서 Render한다.

*/


NS_BEGIN(Engine)

class ENGINE_DLL CGameInstance final: public CBase
{
     DECLARE_SINGLETON(CGameInstance)

private:
    CGameInstance();
    virtual ~CGameInstance() = default;

#pragma region Engine
public:
    HRESULT Initialize_Engine(const ENGINE_DESC & EngineDesc,_Inout_ ID3D11Device** ppDevice, _Inout_ ID3D11DeviceContext * *ppContext);
    
    void    Update_Engine(_float fTimedelta);
    HRESULT     Draw_Begin(const _float4 * pClearColor);
    HRESULT     Draw();
    HRESULT     Draw_End();
    void        Clear(_uint iLevelID);        /*레벨의 자원 삭제.*/
#pragma endregion


#pragma region LevelManager
public:
    HRESULT			Level_Changer(_uint iSceneID, class CLevel* pNewLevel,LEVELCHANGETYPE eChangeType);
    void            Pop_Level();
    class CLevel*   Get_CurrentLevel();
#pragma endregion

#pragma region TimerManager
public:
    _float			Get_TimeDelta(const _tchar * pTimerTag);
    HRESULT			Add_Timer(const _tchar * pTimerTag);
    void			Compute_TimeDelta(const _tchar * pTimerTag);
#pragma endregion


private:
    class CLevel_Manager* m_pLevelManager = { nullptr };
    class CTimer_Manager* m_pTimerManager = { nullptr };
    class CGraphic_Device* m_pGraphicDev = { nullptr };

public:
    virtual void        Free() override;

};

NS_END


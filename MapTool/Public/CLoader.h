#pragma once

#include "CBase.h"
#include "Client_Defines.h"


/*로더 : 직접 로딩을 수행하는 클래스.
서브 스레드가 로딩을 수행한다.
iNextLevel에 맞는 로딩을 수행
*/

namespace Engine
{
    class CGameInstance;
}

NS_BEGIN(MapTool)
class CLoader final:
    public CBase
{
private:
    CLoader(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext);
    virtual ~CLoader() = default;



public:
    HRESULT     Initialize(Client::LEVEL_ID iNextLevelID);
    HRESULT     Loading();              //실제 로딩하는 함수
    void            Output();

private:
    HRESULT        Loading_MapTool();


public:
    _bool       IsFinished() const { return m_isFinished; }


private:
    ComPtr<ID3D11Device>    m_pDevice = { nullptr };
    ComPtr<ID3D11DeviceContext>    m_pDeviceContext = { nullptr };
    CGameInstance* m_pGameInstance = { nullptr };

    LEVEL_ID                        m_iNextLevelID = { LEVEL_ID::END };


private:
    /*스레드 관련*/
    HANDLE      m_hThred = {};
    CRITICAL_SECTION        m_CriticalSection = {};

    _bool                   m_isFinished = { false };   //로딩완료여부
    _tchar                  m_szFPS[MAX_PATH] = {};     //로딩 시 표시할 문자열

public:
    static CLoader* Create(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pDeviceContext, LEVEL_ID iNextLevelID);
    virtual void Free();

};

NS_END
#pragma once
#include "CBase.h"
#include "Client_Defines.h"

namespace Engine
{
    class CGameObject;
    class CLayer;
    class CGameInstance;

}
NS_BEGIN(Client)

class CRoom_Manager :
    public CBase
{
    DECLARE_SINGLETON(CRoom_Manager)

private:
    explicit CRoom_Manager() {};
    virtual ~CRoom_Manager() = default;


public:
    void        Request_Room(const string& name) { m_RequestRoomID = name; }
    const string& Get_RequestRoom() { return m_RequestRoomID; }


                    //CLevel_Room이 초기생성되었을때 각레이어를 참조하게한다.
    HRESULT        Initialize(ComPtr<ID3D11Device> _pDevice, ComPtr<ID3D11DeviceContext> _pContext,
                                    CLayer* pEnv, CLayer* pNPC, CLayer* pTrigger);
    _float4         Get_SpawnPosition() { return m_vSpawnPosition; }
                    //방바꾸기. 캐시된 방중에 업승면 새로 로드하고 저장한다.
    void            Switch_Room(const string& strRoomName);

                //캐시비우기
    void            Clear_Cache();

private:                                //룸이름+.json을 로드한 후 pacakge의 값을채운다.
    HRESULT         Load_Room_From_Json(const string& strRoomName, RoomPackage* pOutPackage);
    void             Clear_Room();  //방 switch 시 퇴장처리
    void             Enter_Room(RoomPackage* pPackage); //방 switch 시 입장(실제적인 오브젝트 설정)
    HRESULT           Load_NPC(const string& RoomName,const wstring& ModelName, _float3 vPos, RoomPackage* pOut);

private:
    CLayer*         m_pEnviromentLayer = nullptr;       //Level_Room의 enviromnetlayer(참조)
    CLayer*         m_pNPCLayer= nullptr;               //Level_Room의 NPClayer(참조)
    CLayer*         m_pTriggerLayer = nullptr;       //Level_Room의 Playerlayer(참조)

public:
    virtual         void        Free() override;
    
private:
    string          m_strCurrentRoomID = "";
    string          m_RequestRoomID = "";       //이동해달라고 요청받은 룸아이디.
                                                //이걸기반으로 switch_room()

    UMap<size_t, RoomPackage*>      m_mapCachedRooms;
    _float4                         m_vSpawnPosition;
    CGameInstance*              m_pGameInstance = nullptr;


private:
    ComPtr<ID3D11Device> m_pDevice;
    ComPtr<ID3D11DeviceContext>     m_pContext;
};

NS_END


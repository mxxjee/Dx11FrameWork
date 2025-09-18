#include "CMainApp.h"
#include "CGameInstance.h"


USING(Client)

CMainApp::CMainApp()
	:pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(pGameInstance);
}

HRESULT CMainApp::Initialize()
{
	/*내 게임의 초기화 작업을 수행*/


	ENGINE_DESC		desc;
	desc.hWnd = g_hWnd;
	desc.iWinSizeX = g_iWinSizeX;
	desc.iWinSizeY = g_iWinSizeY;
	desc.winMode = WINMODE::WIN;


	if(FAILED(pGameInstance->Initialize_Engine(desc,&m_pDevice,&m_pContext)))
		return E_FAIL;


	pGameInstance->Level_Changer(ENUM_TO_UINT(LEVEL_ID::LOGO), nullptr);


	return S_OK;
}

void CMainApp::Update(_float fTimeDelta)
{
	/*내 게임의 반복적인 작업 수행*/
}

void CMainApp::Render()
{
	/*내 게임의 반복적인 렌더.*/
	pGameInstance->Draw_Begin(&ClearColor);
	pGameInstance->Draw();
	pGameInstance->Draw_End();

}

CMainApp* CMainApp::Create()
{
	//메인앱을 완벽하게 생성하기 위한 함수
	//new 뿐만아니라 초기화작업(Initialize)수행
	CMainApp* pMainApp = new CMainApp;

	if (FAILED(pMainApp->Initialize()))
	{
		/*생성하지 못했다면 Safe_Release()로 삭제*/
		MSG_BOX("Failed to Create : CMainApp");
		Safe_Release(pMainApp);
	}

	return pMainApp;
}

void CMainApp::Free()
{
	//자신의 리소스정리

	Safe_Release(pGameInstance);

	//상속계층을 따르기 위해 부모  Free호출 
	__super::Free();

}





#include <iostream>
#include <memory>
using namespace std;

struct Weapon {
	string name;
	Weapon(string n) : name(n) {
		cout << "Weapon " << name << " created\n";
	}
	~Weapon() {
		cout << "Weapon " << name << " destroyed\n";
	}
};

int main() {
	shared_ptr<Weapon> w1;
	{
		auto w2 = make_shared<Weapon>("Excalibur");
		cout << "use_count after make_shared = " << w2.use_count() << "\n";
					// use_count after make_shared = 1

		w1 = w2;  // w1과 w2가 같은 무기를 공유
		cout << "use_count after assigning w1 = " << w2.use_count() << "\n";
					// use_count after make_shared = 2
	}
	cout << "use_count after w2 scope ends = " << w1.use_count() << "\n";
						//use_count after w2 scope ends = 1

	w1.reset();
	cout << "use_count after reset = " << w1.use_count() << "\n";
}

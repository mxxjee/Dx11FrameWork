#ifndef Engine_Enum_h__
#define Engine_Enum_h__

namespace Engine
{
	enum class WINMODE { FULL, WIN }; 

	enum class LEVELCHANGETYPE { NONE,REPLACETOP, OVERLAY, PUSH,END};
	/*REPLACETOP : 이전씬 지우고 TOP교체
	PUSH - 이전씬 냅두고 그냥 얹기(UPDATE/RENDER 모두 비활성화)
	OVERLAY - 이전씬 냅두고 얹기(UPDATE비활성화, RENDER 활성화)
	LOADING - 이번씬은 임시씬이라 이후 무조건지울거에요*/


	enum class LEVELSTATE{ACTIVE,PAUSE,HIDDEN,END};
	/*ACTIVE : UPDATE/RENDER모두 활성화
	* PAUSE : UPDATE(X) /RENDER(O)
	* HIDDEN : UPDATE(X) / RENDER(X)
	*/

	enum class LEVELFLAG {NORMAL, TRANSIENT};
	/*Nomral : 그냥 일반씬
	TRANSIENT : 1회 후 바로 삭제되는 씬 (일회용 씬)*/


	enum class PROTOTYPE
	{
		GAMEOBJECT, COMPONENT
	};
}
#endif // Engine_Enum_h__
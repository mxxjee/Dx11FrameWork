#ifndef Engine_Enum_h__
#define Engine_Enum_h__

namespace Engine
{
	enum class WINMODE { FULL, WIN }; 

	enum class LEVELCHANGETYPE { REPLACETOP, OVERLAY, PUSH,LOADING,END};
	/*REPLACETOP : 이전씬 지우고 TOP교체
	PUSH - 이전씬 냅두고 그냥 얹기(UPDATE/RENDER 모두 비활성화)
	OVERLAY - 이전씬 냅두고 얹기(UPDATE비활성화, RENDER 활성화)
	LOADING - 이번씬은 임시씬이라 이후 무조건지울거에요*/


	enum class LEVELSTATE{ACTIVE,PAUSE,HIDDEN,LOADING,END};
	/*ACTIVE : UPDATE/RENDER모두 활성화
	* PAUSE : UPDATE(X) /RENDER(O)
	* HIDDEN : UPDATE(X) / RENDER(X)

	*/
}
#endif // Engine_Enum_h__
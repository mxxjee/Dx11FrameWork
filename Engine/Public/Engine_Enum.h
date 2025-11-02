#ifndef Engine_Enum_h__
#define Engine_Enum_h__

namespace Engine
{
	enum class WINMODE { FULL, WIN };
	enum class STATE { RIGHT, UP, LOOK, POSITION, END };
	enum class DIRECTION {
		FORWARD, RIGHT, UP, BACKWARD, LEFT, DOWN,

		RIGHTUP, RIGHTDOWN,
		LEFTUP, LEFTDOWN, END
	};

	enum class Space : UINT8
	{
		Local = 1 << 0,
		WORLD = 1 << 1,
		END
	};

	enum class SRTType { SCALE, ROTATION, TRANSFORM, END };
	enum class LEVELCHANGETYPE { NONE, REPLACETOP, OVERLAY, PUSH, END };
	/*REPLACETOP : 이전씬 지우고 TOP교체
	PUSH - 이전씬 냅두고 그냥 얹기(UPDATE/RENDER 모두 비활성화)
	OVERLAY - 이전씬 냅두고 얹기(UPDATE비활성화, RENDER 활성화)
	LOADING - 이번씬은 임시씬이라 이후 무조건지울거에요*/


	enum class LEVELSTATE { ACTIVE, PAUSE, HIDDEN, END };
	/*ACTIVE : UPDATE/RENDER모두 활성화
	* PAUSE : UPDATE(X) /RENDER(O)
	* HIDDEN : UPDATE(X) / RENDER(X)
	*/

	enum class LEVELFLAG { NORMAL, TRANSIENT };
	/*Nomral : 그냥 일반씬
	TRANSIENT : 1회 후 바로 삭제되는 씬 (일회용 씬)*/


	enum class PROTOTYPE
	{
		GAMEOBJECT, COMPONENT
	};

	enum class TransformScope {
		LOCAL, //로컬 기준  ,
		WORLD,//부모행렬이 모두 계산된 전역좌표 
		END
	};
#pragma region KeyCode
	//KeyCode 입력 처리용 키 코드 열거형

	enum class KeyCode : uint16_t
	{
		None = 0,

		// 알파벳
		A = DIK_A, B = DIK_B, C = DIK_C, D = DIK_D, E = DIK_E,
		F = DIK_F, G = DIK_G, H = DIK_H, I = DIK_I, J = DIK_J,
		K = DIK_K, L = DIK_L, M = DIK_M, N = DIK_N, O = DIK_O,
		P = DIK_P, Q = DIK_Q, R = DIK_R, S = DIK_S, T = DIK_T,
		U = DIK_U, V = DIK_V, W = DIK_W, X = DIK_X, Y = DIK_Y, Z = DIK_Z,

		// 숫자
		Num0 = DIK_0, Num1 = DIK_1, Num2 = DIK_2, Num3 = DIK_3,
		Num4 = DIK_4, Num5 = DIK_5, Num6 = DIK_6, Num7 = DIK_7,
		Num8 = DIK_8, Num9 = DIK_9,

		// 방향키 및 특수키
		LeftArrow = DIK_LEFT,
		RightArrow = DIK_RIGHT,
		UpArrow = DIK_UP,
		DownArrow = DIK_DOWN,
		Space = DIK_SPACE,
		Enter = DIK_RETURN,
		Escape = DIK_ESCAPE,
		LShift = DIK_LSHIFT,
		RShift = DIK_RSHIFT,
		LControl = DIK_LCONTROL,
		RControl = DIK_RCONTROL,
		Tab = DIK_TAB,
		Backspace = DIK_BACK,
		Delete = DIK_DELETE,
		Insert = DIK_INSERT,

		// 함수키
		F1 = DIK_F1, F2 = DIK_F2, F3 = DIK_F3, F4 = DIK_F4,
		F5 = DIK_F5, F6 = DIK_F6, F7 = DIK_F7, F8 = DIK_F8,
		F9 = DIK_F9, F10 = DIK_F10, F11 = DIK_F11, F12 = DIK_F12,

		// 키패드 숫자
		Numpad0 = DIK_NUMPAD0, Numpad1 = DIK_NUMPAD1, Numpad2 = DIK_NUMPAD2,
		Numpad3 = DIK_NUMPAD3, Numpad4 = DIK_NUMPAD4, Numpad5 = DIK_NUMPAD5,
		Numpad6 = DIK_NUMPAD6, Numpad7 = DIK_NUMPAD7, Numpad8 = DIK_NUMPAD8,
		Numpad9 = DIK_NUMPAD9,

		// 기타
		Home = DIK_HOME,
		End = DIK_END,
		PageUp = DIK_PRIOR,
		PageDown = DIK_NEXT,
		CapsLock = DIK_CAPSLOCK,

		Count = 256

	};
#pragma endregion
	enum class EInputState : UINT32
	{
		None = 0,
		Pressed = 1 << 0,
		Held = 1 << 1,
		Released = 1 << 2,
	};

	enum class CAMERA_TYPE
	{
		TARGET,
		FREE,
		CUTSCENE,
		SHADOW,
		MINIMAP,
		POSTPROCESS,
		UI,	//항상 마지막이여야함.
		END
	};


	enum class CAMERA_FLAG :uint32_t
	{
		NONE = 0,
		USE_POSTEFFECT = 1 << 0,
		END
	};
	ENABLE_BITMASK_OPERATORS(CAMERA_FLAG);

	enum class COMPONENT_TYPE {
		TRANSFORM,
		VIBUFFER_RECT,
		VIBUFFER_TERRAIN,
		VIBUFFER_TRIANGLE,
		SPHERE_COLLIDER,
		BOX_COLLIDER,
		MESH_COLLIDER,
		MODEL,
		UI
	};

	enum class D3DTS { VIEW, PROJ, END };

	//애니메이션 사용여부(조합을 위해 비트마스크)
	enum class UIAnimFlag : uint8_t
	{
		UI_ANIM_NONE = 0,
		UI_ANIM_SCALE = 1 << 0,
		UI_ANIM_POSITION = 1 << 1,
		UI_ANIM_ROTATION = 1 << 2,
		UI_ANIM_ALPHA = 1 << 3

	};
	ENABLE_BITMASK_OPERATORS(UIAnimFlag);
	inline bool HasFlag(UIAnimFlag value, UIAnimFlag flag)
	{
		return (static_cast<uint8_t>(value) & static_cast<uint8_t>(flag)) != 0;
	}

	enum class UIAnimType
	{
		SCALE,
		POSITION,
		ROTATION,
		ALPHA,
		END,
	};

	//클라이언트에서 파싱할때 나눌 클래스..?
	enum class MapObjType
	{
		OBSTACLE,	//단지 맵에 설치할 조형물들
		TERRAIN,		//지형 관련은 따로처리하자.
		TILE,	//타일관련
		POSITION, //위치값만 존재(EMPTY, 몬스터스폰위치 등)
		TRIGGER,
		END
	};

	//맵툴에서 설치할때 부를 리소스 타입
	enum class ResourceType
	{
		NONE,
		MODEL,
		TEXTURE,
		END
	};



	enum class LIGHT
	{
		DIRECTIONAL, POINT, END
	};

	enum class MaterialMapType
	{
		DIFFUSE,
		AMBIENT,
		NORMAL,
		SPECULAR,
		END
	};
}
#endif // Engine_Enum_h__
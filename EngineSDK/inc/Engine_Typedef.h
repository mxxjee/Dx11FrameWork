#ifndef Engine_Typedef_h__
#define Engine_Typedef_h__



namespace Engine
{
	typedef		bool						_bool;

	typedef		char						_char;   
	typedef		int8_t						_byte;
	typedef		uint8_t						_ubyte;

	typedef		wchar_t						_tchar;

	typedef		signed short				_short;
	typedef		unsigned short				_ushort;

	typedef		signed int					_int;
	typedef		unsigned int				_uint;

	typedef		signed long					_long;
	typedef		unsigned long				_ulong;

	typedef		float						_float;
	typedef		double						_double;

	typedef		wstring						_wstring;


	/*저장용 데이터*/
	typedef		XMFLOAT2					_float2;
	typedef		XMFLOAT3					_float3;
	typedef		XMFLOAT4					_float4;
	typedef		XMFLOAT4X4					_float4x4;


	typedef	  DirectX::SimpleMath::Matrix		Matrix;


	/*unordered_map*/
	template<typename K, typename V> using UMap = std::unordered_map<K, V>;

	/*연산용 데이터 (Vector4)*/
	typedef XMVECTOR		_vector;
	typedef FXMVECTOR		_fvector;
	typedef GXMVECTOR		_gvector;
	typedef HXMVECTOR		_hvector;
	typedef CXMVECTOR		_cvector;

	typedef XMMATRIX		_matrix;
	typedef FXMMATRIX		_fmatrix;
	typedef CXMMATRIX		_cmatrix;




}

#endif // Engine_Typedef_h__

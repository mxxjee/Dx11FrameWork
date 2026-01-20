#pragma once

template<typename T>
class ConstantBuffer
{
public:
	ConstantBuffer(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> context)
		:_device(device), _context(context)
	{

	}

	~ConstantBuffer() =default ;

	ComPtr<ID3D11Buffer> GetComPtr() { return _ConstantBuffer; }
	void	Create()
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Usage = D3D11_USAGE_DYNAMIC;
		//Cpu Write + GPU Read

		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		//용도 : 상수버퍼
		desc.ByteWidth = sizeof(T);
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		HRESULT hr = _device->CreateBuffer(&desc, nullptr, _ConstantBuffer.GetAddressOf());
		CHECK(hr);
	}

	void CopyData(const T& data)		//상수버퍼를 셰이더 (GPU)로 전송
	{
		D3D11_MAPPED_SUBRESOURCE SubResoure;
		ZeroMemory(&SubResoure, sizeof(D3D11_MAPPED_SUBRESOURCE));

		//MAP : [GPU에게 데이터를 넘겨줄 준비를 한다.]
		//직접 ConstantBuffer을 채우는것이 아닌, Subresource(Map을 통해 CPU가 임시 접근할 수 있는공간)에 복사
		_context->Map(_ConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResoure);

		//SubResourceData에 값을 채워준다.
		memcpy(SubResoure.pData, &data, sizeof(T));

		//Unamp : 이를 닫는다.
		_context->Unmap(_ConstantBuffer.Get(), 0);

	}
private:
	ComPtr<ID3D11Device> _device;
	ComPtr<ID3D11DeviceContext> _context;
	ComPtr<ID3D11Buffer> _ConstantBuffer;

};


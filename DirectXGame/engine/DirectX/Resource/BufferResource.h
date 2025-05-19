#pragma once
#include<wrl.h>
#include<d3d12.h>
#include<dxgi1_6.h>
#include<dxcapi.h>
using namespace Microsoft::WRL;

class DirectXCommon;
class BufferResource {
public:


private:
	DirectXCommon* dxCommon;

	Microsoft::WRL::ComPtr < ID3D12Resource> resource_;



};
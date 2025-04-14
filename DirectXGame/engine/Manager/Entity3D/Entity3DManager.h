#pragma once


#include"memory"

class DirectXCommon;
class Entity3DManager
{
public:

	// 初期化
	void Initialize(DirectXCommon* directXCommon);



private:

	// DirectX
	DirectXCommon* directXCommon_;


};


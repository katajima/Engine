#pragma once

// C++
#include"memory"

// engine
#include"DirectXGame/engine/effect/Ocean/OceanManager.h"
#include"DirectXGame/engine/3d/Object/Object3dInstansManager.h"


class DirectXCommon;
class Entity3DManager
{
public:

	// 初期化
	void Initialize(DirectXCommon* directXCommon);

	OceanManager* GetOceanManager() { return oceanManager_.get(); };


	Object3dInstansManager* GetObject3dInstansManager() { return object3dInstansManager_.get(); }
private:

	// DirectX
	DirectXCommon* directXCommon_;

	// オーシャンシェーダー
	std::unique_ptr<OceanManager> oceanManager_ = nullptr;

	// インスタンシング描画
	std::unique_ptr<Object3dInstansManager> object3dInstansManager_ = nullptr;

};


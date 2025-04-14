#pragma once

// C++
#include"memory"

// engine
#include"DirectXGame/engine/effect/Ocean/OceanManager.h"
#include"DirectXGame/engine/3d/Object/Object3dInstansManager.h"
#include"DirectXGame/engine/3d/Object/Object3dCommon.h"
#include "DirectXGame/engine/SkyBox/SkyBoxCommon.h"

class DirectXCommon;
class Entity3DManager
{
public:

	// 初期化
	void Initialize(DirectXCommon* directXCommon);

	OceanManager* GetOceanManager() { return oceanManager_.get(); };


	Object3dInstansManager* GetObject3dInstansManager() { return object3dInstansManager_.get(); }

	Object3dCommon* GetObject3dCommon() { return object3dCommon_.get(); }

	SkyBoxCommon* GetSkyBoxCommon() { return skyBoxCommon_.get(); }

private:

	// DirectX
	DirectXCommon* directXCommon_;

	// オーシャンシェーダー
	std::unique_ptr<OceanManager> oceanManager_ = nullptr;

	// インスタンシング描画
	std::unique_ptr<Object3dInstansManager> object3dInstansManager_ = nullptr;

	// オブジェクト
	std::unique_ptr<Object3dCommon> object3dCommon_ = nullptr;

	// スカイボックス
	std::unique_ptr<SkyBoxCommon> skyBoxCommon_ = nullptr;
	
};


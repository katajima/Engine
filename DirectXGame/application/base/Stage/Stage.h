#pragma once

// C++
#include <imgui.h>
#include <list>


// engine
#include"DirectXGame/engine/Camera/Camera.h"
#include"DirectXGame/engine/3d/Object/Object3d.h"
#include"DirectXGame/engine/2d/Sprite.h"
#include"DirectXGame/engine/base/Imgui/ImGuiManager.h"
#include"DirectXGame/engine/math/MathFanctions.h"
#include"DirectXGame/engine/input/Input.h"
#include"DirectXGame/engine/effect/Ocean/Ocean.h"
#include"DirectXGame/engine/effect/Particle/ParticleManager.h"
#include"DirectXGame/engine/effect/Particle/ParticleEmitter.h"


class DirectXCommon;
class Entity3DManager;
class Entity2DManager;
class Stage {
public:
	// 初期化
	void Initialize(DirectXCommon* dxcommon, Entity3DManager* entity3DManager, Entity2DManager* entity2DManager,Camera* camera);

	// 更新
	void Update();

	///< summary>
	/// 描画
	///</summary>
	void Draw();

	void DrawP();

	void Draw2D();

private:
	// 空
	std::unique_ptr<Object3d> sky_;

	// 地面
	std::unique_ptr < Object3d> tail_;
	std::unique_ptr < Object3d> tail2_;
	// 海
	// オーシャンシェーダー
	std::unique_ptr <Ocean> ocean_ = nullptr;

	// 工場
	std::vector < std::unique_ptr<Object3d>> warehouseObject_;
	// 建物オブジェクト位置
	std::vector<Vector3> warePos;

private:

	std::unique_ptr<ParticleEmitter> emit_;
private:
	DirectXCommon* dxCommon_;
	Entity3DManager* entity3DManager_;
	Entity2DManager* entity2DManager_;

	Camera* camera_ = nullptr;

};

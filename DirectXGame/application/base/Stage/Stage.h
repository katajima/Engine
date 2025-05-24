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
	
	// 列車
	std::unique_ptr < Object3d> train_;
	float trainSpeed_ = 300.0f;
	float trainStartX_ = -3110.0f;
	float trainEndX_ = 4000.0f;
	float trainWarpTime_ = 0.0f;
	float trainWarpTimeMax_ = 20.0f;




	std::unique_ptr<SkyBox> skyBox;

	// ライト
	std::shared_ptr<PointLight> pointLight_;
private:

	std::unique_ptr<ParticleEmitter> emit_;

	std::unique_ptr<ParticleEmitter> emitTrainDust_;
private:
	DirectXCommon* dxCommon_;
	Entity3DManager* entity3DManager_;
	Entity2DManager* entity2DManager_;

	Camera* camera_ = nullptr;

};

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

#include "DirectXGame/application/base/BaseClass/Light/BaseLights.h"

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

	void InitEmit();

	void CloudEmit(ParticleManager* particleManager,ParticleEmitter* emit,const std::string& name);

	void EmitUpdate();

public:
	std::vector<Object3d*> missiles_;
private:
	// 空
	Object3d* sky_;

	
	// 地面
	Object3d* tail_;
	

	// 列車
	Object3d* train_;
	float trainSpeed_ = 300.0f;
	float trainStartX_ = -3110.0f;
	float trainEndX_ = 4000.0f;
	float trainWarpTime_ = 0.0f;
	float trainWarpTimeMax_ = 20.0f;

	// 船
	Object3d* ship_;
	Vector3 velocity_ = { 0.0f,0.0f,0.0f };

	// 円運動のパラメータ
	float radius = 1000.0f; // 円の半径
	float angularSpeed = DirectX::XM_PI * 0.5f * 0.25f; // ラジアン/秒（90度/秒）
	
	Vector3 center = { -3111,-50,3040 }; // 中心座標を指定


	// オーシャンシェーダー
	std::unique_ptr<Ocean> ocean_ = nullptr;
	Object3d* oceanObject;


	// スカイボックス
	std::unique_ptr<SkyBox> skyBox;
	

	// ライト
	std::shared_ptr<DirectionalLight> directional;
private:

	// ステージに舞う埃
	std::unique_ptr<ParticleEmitter> emit_;

	// 列車の煙
	std::unique_ptr<ParticleEmitter> emitTrainDust_;

	// 船の水しぶき
	std::unique_ptr<ParticleEmitter> emitShipDust_;

	// 雲の粒子
	std::unique_ptr<ParticleEmitter> emitCloudDust_;
	std::unique_ptr<ParticleEmitter> emitCloudDust2_;
	std::unique_ptr<ParticleEmitter> emitCloudDust3_;



private:
	DirectXCommon* dxCommon_;
	Entity3DManager* entity3DManager_;
	Entity2DManager* entity2DManager_;

	Camera* camera_ = nullptr;

};

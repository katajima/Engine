#pragma once
// engine
#include"DirectXGame/engine/scene/SceneManager.h"
#include"DirectXGame/engine/collider/3d/CollisionManager.h"
#include"DirectXGame/engine/Utility/TimerUtility.h"
#include"DirectXGame/engine/base/Load/LoadLevelData.h"

// application
#include"DirectXGame/application/GlobalVariables/GlobalVariables.h"
#include"DirectXGame/application/base/Camera/FixedCamera/EffectCamera.h"


#include"DirectXGame/application/base/Camera/Base/CameraManager.h"
#include"DirectXGame/application/base/Effect/Effect.h"
#include "DirectXGame/application/base/Input/InputCoordinator.h"

#include "DirectXGame/engine/Effect/Particle/2d/ParticleEmitter2d.h"
#include "DirectXGame/engine/Light/Lights.h"
#include "DirectXGame/engine/SkyBox/SkyBox.h"

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

class EffectScene : public Engine::BaseScene {
public:

	// 初期化
	void Initialize() override;

	// 終了
	void Finalize() override;

	// 毎フレーム更新
	void Update() override;

	// 描画3d
	void Draw3D() override;
	// 描画2d
	void Draw2D() override;
private:
	// エフェクト確認用カメラを生成して描画系へ接続する
	void InitializeCamera();
	// エフェクトを見やすくする空とライトを生成する
	void InitializeEnvironment();
	// 試射位置を示すリングなどの補助オブジェクトを生成する
	void InitializePreviewObjects();
	// シーン内で自動再生するエフェクト情報を登録する
	void InitializePreviewEffects();
	// 2Dパーティクルの確認用エミッターを生成する
	void InitializeParticle2D();
	// 入力による単発発火やシーン移動を更新する
	void UpdateInput();
	// 登録済みエフェクトをタイマーで順番に発火する
	void UpdateAutoEmit(float dt);
	// 補助オブジェクトのアニメーションと表示状態を更新する
	void UpdatePreviewObjects(float dt);
	// デバッグ用のImGui操作パネルを描画する
	void UpdateImGui();
	// 指定された試射スロットのエフェクトを発火する
	void EmitPreview(size_t index);
	// 登録済みエフェクトを一括で発火する
	void EmitAllPreview();

private:
	// 1つの確認用エフェクト発火地点をまとめる構造体
	struct EffectPreviewSlot {
		std::string name;					// 発火するエミッター名
		Vector3 position{};					// 発火位置
		Vector3 velocity{};					// 方向指定ありの場合の中心速度
		Vector3 velocityRange{};			// 方向指定ありの場合の速度揺らぎ
		float interval = 1.0f;				// 自動発火間隔
		float timer = 0.0f;					// 自動発火用の経過時間
		bool useVelocity = false;			// 速度指定付きEmitを使うか
		Engine::Object3d* marker = nullptr;	// 発火位置を示す非所有ポインタ
	};

private:
	// 物理入力、アクション入力、コマンドをまとめて管理する
	std::unique_ptr<InputCoordinator> inputCoordinator_ = nullptr;

	// カメラ管理
	std::unique_ptr<CameraManager> cameraManager_ = nullptr;
	// エフェクト用カメラ
	std::unique_ptr<EffectCamera> effectCamera_ = nullptr;

	// エフェクト
	std::unique_ptr<EffectSystem> effect_;
	// 2Dパーティクル確認用エミッター
	std::unique_ptr<Engine::ParticleEmitter2d> particleEmitter2d_ = nullptr;
private:
	// スカイボックス
	std::unique_ptr<Engine::SkyBox> skyBox = nullptr;
	// 空
	Engine::Object3d* sky_ = nullptr;

	// 空スケール
	Vector3 skyBoxScale_ = { 100,100,100 };
	// 平行光源
	std::shared_ptr<Engine::DirectionalLight> directionalLight_ = nullptr;
	// ポイントライト
	std::shared_ptr<Engine::PointLight> pointLight_ = nullptr;
	// スポットライト
	std::shared_ptr<Engine::SpotLight> spotLight_ = nullptr;

private:
	// 確認用エフェクトの発火スロット一覧
	std::vector<EffectPreviewSlot> previewSlots_;
	// 手動発火するスロット番号
	size_t selectedPreviewIndex_ = 0;
	// 自動発火を行うか
	bool isAutoEmit_ = true;
	// 全体発火の周期タイマー
	float allEmitTimer_ = 0.0f;
	// 全体発火の周期
	float allEmitInterval_ = 5.0f;
	// 補助オブジェクトのアニメーション時間
	float previewAnimationTimer_ = 0.0f;
};

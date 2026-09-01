#pragma once
// engine
#include<DirectXGame/engine/Effect/EffectComponent.h>
#include "DirectXGame/engine/Light/BaseLights.h"
#include "DirectXGame/application/base/Car/PlayerCar.h"
#include "DirectXGame/engine/Base/Load/LoadLevelData.h"
#include "DirectXGame/application/base/Stage/StageColliderSystem.h"
#include "DirectXGame/engine/Math/Random.h"

#include "DirectXGame/engine/SkyBox/SkyBox.h"
#include "DirectXGame/engine/Effect/Ocean/Ocean.h"
#include <DirectXGame/engine/Effect/Primitive/Primitive.h>

namespace Engine {
	class EntityManager;
	class Object3d;
	class Camera;
	class AudioManager;
}

/// <summary>
/// ステージ基礎クラス
/// </summary>
class BaseStage {
public:
	~BaseStage() {
		if (loadData_) {
			loadData_->ClearData();
		}
	}

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Initialize(Engine::EntityManager* entityManager,Engine::AudioManager* audioManager, CameraManager* cameraManager) = 0;

	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update(float dt) = 0;

	///< summary>
	/// 描画
	///</summary>
	virtual void Draw() {};

	/// <summary>
	/// 描画エフェクト
	/// </summary>
	virtual void DrawEffect() {};

	/// <summary>
	/// 描画2d
	/// </summary>
	virtual void Draw2D() {};


public:
	/// <summary>
	/// レベルデータ取得
	/// </summary>
	LoadLevelData* GetLoadLevelData() { return loadData_.get(); }
	/// <summary>
	/// ステージコライダーシステム取得
	/// </summary>
	StageColliderSystem* GetStageColliderSystem() { return stageColliderSystem_.get(); }
protected:

	/// <summary>
	/// 海初期化
	/// </summary>
	void InitializeOcean(Engine::EntityManager* entityManager, CameraManager* cameraManager);

	/// <summary>
	/// 空初期化
	/// </summary>
	void InitializeSky(Engine::EntityManager* entityManager, CameraManager* cameraManager);

	/// <summary>
	/// ライト初期化
	/// </summary>
	void InitializeLight(Engine::EntityManager* entityManager);

	/// <summary>
	/// レベルデータ初期化
	/// </summary>
	void InitializeLevelData(Engine::EntityManager* entityManager, CameraManager* cameraManager,const std::string& name);

	/// <summary>
	/// ステージコライダーシステム初期化
	/// </summary>
	/// <param name="entityManager"></param>
	void InitializeStageColliderSystem(Engine::EntityManager* entityManager);

	/// <summary>
	/// 共通の地面オブジェクトを初期化する。
	/// </summary>
	/// <param name="entityManager">エンティティ管理。</param>
	/// <param name="scale">地面とマテリアルへ設定する拡大率。</param>
	void InitializeGround(Engine::EntityManager* entityManager, const Vector3& scale = { 100.0f, 100.0f, 100.0f });

	/// <summary>
	/// プレイヤー車を生成し、共通の接地設定まで初期化する。
	/// </summary>
	/// <param name="entityManager">エンティティ管理。</param>
	/// <param name="playerCar">生成した車を保持する所有ポインタ。</param>
	/// <param name="position">車の初期座標。</param>
	/// <param name="rotation">車の初期回転。</param>
	void InitializePlayerCar(Engine::EntityManager* entityManager, std::unique_ptr<PlayerCar>& playerCar,
		const Vector3& position, const Vector3& rotation);

protected:
	/// <summary>
	/// 基礎更新
	/// </summary>
	/// <param name="dt"></param>
	void BaseUpdate(float dt);

protected:
	/// <summary>
	/// エンティティ管理クラス取得
	/// </summary>
	Engine::EntityManager* GetEntityManager() { return entityManager; }
protected: // 基礎オブジェクト
	// オーシャンシェーダー
	std::unique_ptr<Engine::Ocean> ocean_ = nullptr;
	// オーシャンオブジェクト
	Engine::Object3d* oceanObject = nullptr;
	// スカイボックス
	std::unique_ptr<Engine::SkyBox> skyBox = nullptr;
	// 空
	Engine::Object3d* sky_ = nullptr;
	// 地面
	Engine::Object3d* tail_ = nullptr;
	// ライト
	std::shared_ptr<Engine::DirectionalLight> directional = nullptr;
	// レベルデータ
	std::unique_ptr<LoadLevelData> loadData_ = nullptr;
	// ステージコライダーシステム
	std::unique_ptr<StageColliderSystem> stageColliderSystem_ = nullptr;
protected:	// パラメータ系

	// 海パラメータ
	struct OceanParameters {
		Vector3 translate = { 0, -30, 0 };
		Vector3 rotate = { Math::DegreesToRadians(90) ,0,0 };
		Vector2 range{ 8000.0f,8000.0f };
		float amplitude = 16.5f;
		Vector2 waveDirection = { 0.5f,0.5f };
		float waveSpeed = 5.0f;
		Color color = { 0.0f, 0.0f, 0.8f, 0.75f };
	};
	// 海パラメータ
	OceanParameters oceanParameters_;
	// 空スケール
	Vector3 skyBoxScale_ = { 100,100,100 };
protected: // 貰いもの
	Engine::EntityManager* entityManager = nullptr;
	Engine::Camera* camera = nullptr;
	CameraManager* cameraManager = nullptr;
	Engine::AudioManager* audioManager = nullptr;
	// エフェクト
	EffectSystem* effect = nullptr;
};

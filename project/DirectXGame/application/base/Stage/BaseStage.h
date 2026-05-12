#pragma once
// engine
#include<DirectXGame/engine/Effect/EffectComponent.h>
#include "DirectXGame/application/base/Light/BaseLights.h"
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

	// 初期化
	virtual void Initialize(Engine::EntityManager* entityManager, CameraManager* cameraManager) = 0;

	// 更新
	virtual void Update(float dt) = 0;

	///< summary>
	/// 描画
	///</summary>
	virtual void Draw() = 0;

	/// <summary>
	/// 描画エフェクト
	/// </summary>
	virtual void DrawEffect() = 0;

	/// <summary>
	/// 描画2d
	/// </summary>
	virtual void Draw2D() = 0;


public:
	// レベルデータ取得
	LoadLevelData* GetLoadLevelData() { return loadData_.get(); }
	// ステージコライダーシステム取得
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

protected:
	/// <summary>
	/// 基礎更新
	/// </summary>
	/// <param name="dt"></param>
	void BaseUpdate(float dt);

protected:
	// エンティティ管理クラス取得
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
	// エフェクト
	EffectSystem* effect = nullptr;
};

#pragma once
#include "DirectXGame/engine/Collider/3d/ColliderComponent.h"

namespace Engine {
	class LineCommon;
	class Object3d;
}

/// <summary>
/// ステージのコライダーの管理システム
/// </summary>
class StageColliderSystem : public IHitReceiver {
public:
	/// <summary>
	/// デストラクタ
	/// </summary>
	~StageColliderSystem() {
		colliderComponent_->ClearColliders();
	}
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Engine::LineCommon* lineCommon);

	/// <summary>
	/// オブジェクト加算
	/// </summary>
	void AddObject(Engine::Object3d* object) { objects.push_back(object); }

	/// <summary>
	/// コライダー生成
	/// </summary>
	void CreateCollider();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	Engine::ColliderComponent* GetColliderComponent() { return colliderComponent_.get(); }
private:
	void DebugImGui();
private:
	std::vector<Engine::Object3d*> objects;
	std::unique_ptr<Engine::ColliderComponent> colliderComponent_;
	Engine::LineCommon* lineCommon;
};

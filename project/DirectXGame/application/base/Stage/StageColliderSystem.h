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
	// デストラクタ
	~StageColliderSystem() {
		colliderComponent_->ClearColliders();
	}
	// 初期化
	void Initialize(Engine::LineCommon* lineCommon);

	// オブジェクト加算
	void AddObject(Engine::Object3d* object) { objects.push_back(object); }

	// コライダー生成
	void CreateCollider();

	// 更新
	void Update();

	Engine::ColliderComponent* GetColliderComponent() { return colliderComponent_.get(); }
private:
	void DebugImGui();
private:
	std::vector<Engine::Object3d*> objects;
	std::unique_ptr<Engine::ColliderComponent> colliderComponent_;
	Engine::LineCommon* lineCommon;
};

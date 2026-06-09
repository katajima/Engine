#include "StageColliderSystem.h"
#include"DirectXGame/engine/3d/Object/Object3d.h"

void StageColliderSystem::Initialize(Engine::LineCommon* lineCommon) {
	// ステージ用のコライダー管理コンポーネントを作成する
	colliderComponent_ = std::make_unique<Engine::ColliderComponent>();
	colliderComponent_->SetLineCommon(lineCommon);
	// ステージは基本的に衝突結果を受けるだけなので、コールバックは相手取得までに留める
	GetColliderComponent()->onHitCallback = [this](Engine::Collider* self, Engine::Collider* other) {
		(void)self;
		auto* otherComponent = static_cast<Engine::ColliderComponent*>(other->GetOwner());
		if (!otherComponent) return;
		};
}

void StageColliderSystem::CreateCollider() {

	// ステージモデルの全三角形を静的な壁コライダーとして登録する
	for (auto& obj : objects) {
		for (auto& mesh : obj->GetModel()->GetModelData().mesh) {
			for (auto& triangle : mesh->GetTriangles()) {
				// モデルメッシュの三角形頂点をそのままコライダー形状に使う
				auto triangleColl = std::make_unique<Engine::TriangleCollider>();
				triangleColl->triangle01 = triangle.vertices[0];
				triangleColl->triangle02 = triangle.vertices[1];
				triangleColl->triangle03 = triangle.vertices[2];
				triangleColl->SetTag(CollisionTag::Wall);
				triangleColl->SetLayer(CollisionLayer::Environment);
				triangleColl->SetCollisionMask(0xFFFFFFFF);
				triangleColl->SetIsStatic(true);
				triangleColl->SetIsDebugLine(false);
				triangleColl->isNormal = true;
				colliderComponent_->AddCollider(std::move(triangleColl));
			}
		}
	}
	
}

void StageColliderSystem::Update() {

	// デバッグ表示の切り替えUIを更新する
	DebugImGui();

	// ステージは静的オブジェクトなので、代表オブジェクトのTransformで全コライダーを更新する
	colliderComponent_->UpdateAll(objects[0]->GetWorldTransform());
};

void StageColliderSystem::DebugImGui() {
#ifdef _DEBUG
	// コライダー線の表示/非表示をまとめて切り替える
	ImGui::Begin("Collider");
	if (ImGui::Button("isDebugLine")) {
		for (auto& coll : colliderComponent_->GetAllColliders()) {
			if (coll->IsDebugLine()) {
				coll->SetIsDebugLine(false);
			}
			else {
				coll->SetIsDebugLine(true);
			}
		}
	}
	ImGui::End();
#endif // _DEBUG
};

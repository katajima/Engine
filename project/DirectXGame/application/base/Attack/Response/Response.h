#pragma once
#include "DirectXGame/application/base/Attack/AttackData.h"
#include <DirectXGame/engine/Collider/CollisionTypeIdDef.h>
#include "DirectXGame/application/base/Attack/Hit/HitMotionSystem.h"

// 前方宣言
class ObjectComponent;		// オブジェクト
namespace Engine {
	class WorldTransform;	// オブジェクト位置
	class Collider;			// コライダー
}
class MoveRequestSystem;	// 移動リクエストシステム

/// <summary>
/// 衝突応答クラス（オブジェクトとの接触時の押し戻し処理など）
/// </summary>
class HitResponse {
public:

	// 衝突応答(タグごとの)
	void Hit(CollisionTag tag, Engine::Collider* self, Engine::Collider* other);
	// 壁や床
	void HitWall(Engine::Collider* self, Engine::Collider* other);
	// 影響
	void HitEffect(Engine::Collider* self, Engine::Collider* other);
	// 使っているもののワールドトランスフォームを設定
	void SetOwner(Engine::WorldTransform* owner) { transform = owner; };

	void SetOwner(MoveRequestSystem* owner) { this->moveRequestSystem = owner; };

private:
	Engine::WorldTransform* transform = nullptr;	//　衝突応答用
	MoveRequestSystem* moveRequestSystem = nullptr;	// 衝突応答
	float halfSize = 0.5f;		// 半分のサイズ

};



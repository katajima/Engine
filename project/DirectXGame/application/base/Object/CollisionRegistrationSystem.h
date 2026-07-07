#pragma once
#include <memory>

namespace Engine {
	class GlobalVariables;
	class CollisionManager;
	class LineCommon;
}


namespace HitBox {
	class System;
}

namespace Character {
	class CharacterManager;
}

class SpecialPointManager;
class BulletManager;

/// <summary>
/// コリジョンの衝突登録システム
/// </summary>
class CollisionRegistrationSystem {
public:
	// 初期化
	void Initialize(Engine::GlobalVariables* globalVariables,Engine::LineCommon* lineCommon, HitBox::System* hitBoxSystem,
		Character::CharacterManager* characterManager,	SpecialPointManager* specialPointManager,	BulletManager* bulletManager);

	// 衝突登録と判定
	void RegisterAllCollisions();

	// 衝突マネージャ取得
	Engine::CollisionManager* GetCollisionManager() { return collisionManager_.get(); }

private:
	// 衝突マネージャ
	std::unique_ptr<Engine::CollisionManager> collisionManager_ = nullptr;
	// ライン管理
	Engine::LineCommon* lineCommon = nullptr;
	// ヒットボックスシステム
	HitBox::System* hitBoxSystem = nullptr;
	// キャラクター管理
	Character::CharacterManager* characterManager = nullptr;
	// ポイント管理
	SpecialPointManager* specialPointManager = nullptr;
	// 弾管理
	BulletManager* bulletManager = nullptr;


};

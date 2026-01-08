#pragma once
#include <DirectXGame/engine/Collider/3d/ColliderComponent.h>
#include <DirectXGame/application/base/Attack/AttackData.h>
class BaseCharacter;

// 使用者のタイプ
enum class HitBoxUseType {
	kPlayer,	// プレイヤー
	kEnemy,		// 敵
	kOther,		// その他
};


class HitBoxFunction {
public:

	// 初期化
	void Initialize(Engine::ColliderComponent* owner, BaseCharacter* character, HitBoxUseType type) {
		owner_ = owner; 
		character_ = character;
		type_ = type;
	};

	// 開始
	bool Begin(Engine::Collider* self, Engine::Collider* other);

	// 更新
	void Update();


	void SetData(AttackReactionData data) { data_ = data; };

private:
	// 使用者がプレイヤーの場合の更新処理
	void UpdateTypePlayer();
	// 使用者が敵の場合の更新処理
	void UpdateTypeEnemy();
	// 使用者がキャラクター以外の場合の更新処理
	void UpdateTypeOther();
private:
	// 衝突履歴取得
	Engine::ContactRecord& GetContactRecord() { return owner_->contactRecord_; }
private:
	Engine::ColliderComponent* owner_ = nullptr;	// コライダーコンポーネント(使用者)
	Engine::ColliderComponent* other_ = nullptr;	// コライダーコンポーネント(相手)
	BaseCharacter* character_ = nullptr;	// 使用キャラクター
	HitBoxUseType type_;							// 使用者

	Engine::Collider* otherColl_ = nullptr;
	AttackReactionData data_;
};


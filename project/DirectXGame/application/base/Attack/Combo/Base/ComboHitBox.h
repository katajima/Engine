#pragma once
#include "DirectXGame/engine/struct/Vector3.h"
#include "DirectXGame/application/base/Attack/AttackData.h"
#include <DirectXGame/application/base/Attack/HitBox/HitBoxSystem.h>
#include "ComboGlobalData.h"
#include "ComboCondition.h"

class JumpSystem;			// ジャンプシステム

/// <summary>
/// ヒットボックス
/// </summary>
class ComboHitBox {
public:

	enum class HitBoxSpawnType {
		kOnTime,			// 時間で発生
		kOnGround,		// 着地で発生
		kOnAir,			// 空中から発生
		kOnButtonRelease,// ボタンを離したら
	};


	enum class Shape {
		kAABB,
		kOBB,
		kSphere
	};


	// ヒットボックスデータ
	struct Data {
		float hitBpxWindowStart_ = 1.0f;		// ヒットボックス生成スタート
		float lifeTime_ = 1.0f;					// ヒットボックス生存時間
		// ヒットボックス使用者タイプ
		HitBoxUseType hitBoxUseType_;
		// ヒットボックスの発生条件タイプ
		HitBoxSpawnType spawnType_ = HitBoxSpawnType::kOnTime;
		// ヒットボックス依存先タイプ
		HitBoxSystem::Type dependenceType_ = HitBoxSystem::Type::kParent;
		// オフセット
		Vector3 offset_{};
	};

	// 開始
	void Enter(BaseCharacter* owner);

	// 更新
	void Update(const Engine::Input& input, float timer, float dt);

	// 終了
	void Exit();


public:

	// データ取得
	Data& GetData() { return data_; }
	// コライダーデータ追加
	void AddCollider(const HitBoxCollData& hitBoxData, const ComboGlovalData& reaction);
	// 使うヒットボックス名設定
	void AddUseHitBox(const std::string& name) { useHitBox_.push_back(name); };
	// 使うヒットボックス名クリーン
	void ClearUseHitBox() { useHitBox_.clear(); }
	// 親子設定
	void SetPerent(Engine::WorldTransform* perent) { perent_ = perent; };
	//
	void SetDirection(Vector3 direction) { direction_ = direction; };
private:
	// ヒットボックスシステム
	HitBoxSystem* hitBoxSystem_ = nullptr;
	// ジャンプシステム
	JumpSystem* jumpSystem_ = nullptr;
	// 親子
	Engine::WorldTransform* perent_ = nullptr;
private:
	ComboButton button_ = ComboButton(GamePadButton::GAMEPAD_B, ComboButtonInputType::kPressed); // コンボボタン
	// ヒットボックスデータ
	Data data_;
	// コライダーデータ
	std::vector<HitBoxCollData> collData_;

	// 使うヒットボックス名
	std::vector<std::string> useHitBox_;

private:
	// ヒットボックス出現
	bool isPopHitBox_ = false;
	//
	Vector3 direction_ = {};
	//
	float timer_ = 0.0f;
};


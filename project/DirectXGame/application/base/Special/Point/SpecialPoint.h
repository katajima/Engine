#pragma once
#include "DirectXGame/application/base/Object/ObjectComponent.h"
#include "SpecialPointStateMachine.h"

// 前方宣言
namespace Engine {
	class Input;
	class EntityManager;
}
namespace Character {
	class BaseCharacter;
	class BasePlayer;
}
/// <summary>
/// スペシャルポイント
/// </summary>
class SpecialPoint : public IHitReceiver {
public:

	// 初期化
	void Initialize(Engine::EntityManager* entityManager, Engine::GlobalVariables* globalVariables, const Vector3& pos, int point,int id);
	// 更新
	void Update(float dt);
	// 描画
	void Draw() {};

public:
	// 生きているか
	bool GetAlive() const { return isAlive_; };
	// 死亡
	void Die() { isAlive_ = false; }
	// ターゲット位置設定
	void SetTargetPos(const Vector3& target) { targetPos_ = target; };
	// オブジェクトコンポーネント取得
	ObjectComponent* GetObjectComponent() { return objectComponent_.get(); }
	// コライダーコンポーネント取得
	Engine::ColliderComponent* GetColliderComponent() { return objectComponent_->GetColliderComponent(); }
	// ステートマシーン取得
	SpecialPointStateMachine* GetStateMachine() { return specialPointStateMachine_.get(); }
	// ターゲット位置取得
	Vector3 GetTargetPos() const { return targetPos_; }
	// 移動範囲内か
	bool IsMove();
private:

private:
	// オブジェクトコンポーネント
	std::unique_ptr<ObjectComponent> objectComponent_;			
	// ステートマシーン
	std::unique_ptr<SpecialPointStateMachine> specialPointStateMachine_;
private:
	// 生存フラグ
	bool isAlive_ = true;
	// ポイント量
	int point_ = 1;
	// ID
	int id_ = -1;
	// ターゲット位置
	Vector3 targetPos_ = {};
	// 引き寄せ範囲
	float radius_ = 75.0f;

};


/// <summary>
/// スペシャルポイント管理クラス
/// </summary>
class SpecialPointManager {
public:
	// 初期化
	void Initialize(Engine::EntityManager* entityManager, Engine::GlobalVariables* globalVariables);
	// 更新
	void Update(float dt);
	// ポイント出現
	void AddPoint(const Vector3& pos, int point);
	// ターゲット設定
	void SetTarget(Character::BaseCharacter* target) { this->target = target; };

	// SPポイントたち取得
	std::vector<SpecialPoint*> GetSpecialPoints() {
		std::vector<SpecialPoint*> result;
		for (auto& point : points) {
			result.push_back(point.get());
		}
		return result;
	}
private:
	std::vector<std::unique_ptr<SpecialPoint>> points;	// SPポイントたち
	int count_ = 0;			// 出現回数カウント用
private:
	Character::BaseCharacter* target = nullptr;		// ターゲット
private:
	Engine::EntityManager* entityManager = nullptr;	// エンティティマネージャー
	Engine::GlobalVariables* globalVariables = nullptr;	// グローバル変数
	Engine::Camera* camera = nullptr;						// カメラ
};

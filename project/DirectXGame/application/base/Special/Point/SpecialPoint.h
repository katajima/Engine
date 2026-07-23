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

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Engine::EntityManager* entityManager, Engine::GlobalVariables* globalVariables, const Vector3& pos, int point,int id);
	/// <summary>
	/// 更新
	/// </summary>
	void Update(float dt);
	/// <summary>
	/// 描画
	/// </summary>
	void Draw() {};

public:
	/// <summary>
	/// 生きているか
	/// </summary>
	bool GetAlive() const { return isAlive_; };
	/// <summary>
	/// 死亡
	/// </summary>
	void Die() { isAlive_ = false; }
	/// <summary>
	/// ターゲット位置設定
	/// </summary>
	void SetTargetPos(const Vector3& target) { targetPos_ = target; };
	/// <summary>
	/// オブジェクトコンポーネント取得
	/// </summary>
	ObjectComponent* GetObjectComponent() { return objectComponent_.get(); }
	/// <summary>
	/// コライダーコンポーネント取得
	/// </summary>
	Engine::ColliderComponent* GetColliderComponent() { return objectComponent_->GetColliderComponent(); }
	/// <summary>
	/// ステートマシーン取得
	/// </summary>
	SpecialPointStateMachine* GetStateMachine() { return specialPointStateMachine_.get(); }
	/// <summary>
	/// ターゲット位置取得
	/// </summary>
	Vector3 GetTargetPos() const { return targetPos_; }
	/// <summary>
	/// 移動範囲内か
	/// </summary>
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
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Engine::EntityManager* entityManager, Engine::GlobalVariables* globalVariables);
	/// <summary>
	/// 更新
	/// </summary>
	void Update(float dt);
	/// <summary>
	/// ポイント出現
	/// </summary>
	void AddPoint(const Vector3& pos, int point);
	/// <summary>
	/// ターゲット設定
	/// </summary>
	void SetTarget(Character::BaseCharacter* target) { this->target = target; };

	/// <summary>
	/// SPポイントたち取得
	/// </summary>
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

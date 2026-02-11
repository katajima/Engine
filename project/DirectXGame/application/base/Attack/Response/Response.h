#pragma once
#include "DirectXGame/application/base/Attack/AttackData.h"
#include <DirectXGame/engine/Collider/CollisionTypeIdDef.h>
#include "DirectXGame/application/base/Attack/Hit/HitMotionSystem.h"

// 前方宣言
namespace Character {
	class ParameterComponent;	// キャラクターパラメータ設定
}
class ObjectComponent;				// オブジェクト
class WorldTransform;				// オブジェクト位置
class Collider;						// コライダー



/// <summary>
/// 攻撃を食らったときの応答クラス
/// </summary>
class AttackResponse {
public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Character::ParameterComponent* paremeter, ObjectComponent* object);

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="dt">ゲーム内時間もしくはキャラクターの時間など</param>
	void Update(float dt);

	/// <summary>
	/// ヒットモーションクラス取得
	/// </summary>
	/// <returns></returns>
	HitMotionSystem* GetHitMotionSystem() { return hitMotionSystem_.get(); }

private:
	// ヒットモーション
	std::unique_ptr<HitMotionSystem> hitMotionSystem_ = nullptr;
private:// 貰いもの
	// キャラクターパラメータ
	Character::ParameterComponent* paremeter = nullptr;
	// オブジェクト
	ObjectComponent* object = nullptr;
	// ヒットフラグ<タグ、フラグ>何かヒットしたときに使う用のやつ 
	std::map<std::string, bool> hitFlags_;
};



/// <summary>
/// 衝突応答
/// </summary>
class HitResponse {
public:

	// 衝突応答(タグごとの)
	void Hit(CollisionTag tag, Engine::Collider* self, Engine::Collider* other);

	// 使っているもののワールドトランスフォームを設定
	void SetOwner(Engine::WorldTransform* owner) { transform = owner; };

private:
	Engine::WorldTransform* transform;	//　衝突応答用
	float halfSize = 0.5f;		// 半分のサイズ

};


/// <summary>
/// 攻撃もしくは接触衝突時のシステムクラス
/// </summary>
class ResponseSystem {
public:

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="paremeter">kキャラクターパラメータクラス</param>
	/// <param name="object">オブジェクトクラス</param>
	void Initialize(Character::ParameterComponent* paremeter, ObjectComponent* object);


	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="dt"></param>
	void Update(float dt);

	/// <summary>
	/// 攻撃衝突応答クラス取得
	/// </summary>
	/// <returns></returns>
	AttackResponse* GetAttackResponse() { return attackResponse_.get(); }

	/// <summary>
	/// 接触衝突応答クラス取得
	/// </summary>
	/// <returns></returns>
	HitResponse* GetHitResponse() { return hitResponse_.get(); };


	/// <summary>
	/// ヒットモーションクラス取得
	/// </summary>
	HitMotionSystem* GetHitMotionSystem() { return attackResponse_->GetHitMotionSystem(); }

private:
	// 攻撃衝突応答クラス
	std::unique_ptr<AttackResponse> attackResponse_ = nullptr;
	// 衝突応答クラス(押し戻しなど)
	std::unique_ptr<HitResponse> hitResponse_ = nullptr;	
};


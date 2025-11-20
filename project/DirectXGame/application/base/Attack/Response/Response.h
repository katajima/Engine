#pragma once
#include "DirectXGame/application/base/Attack/AttackData.h"
#include <DirectXGame/engine/Collider/CollisionTypeIdDef.h>

// 前方宣言
class CharacterParameterComponent;	// キャラクターパラメータ設定
class ObjectComponent;				// オブジェクト
class WorldTransform;				// オブジェクト位置
class Collider;						// コライダー

/// <summary>
/// 攻撃を食らったときの応答クラス
/// </summary>
class AttackResponseData {
public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(CharacterParameterComponent* paremeter, ObjectComponent* object);

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="dt">ゲーム内時間もしくはキャラクターの時間など</param>
	void Update(float dt);


	/// <summary>
	/// 攻撃を受けたときに送るデータを挿入
	/// </summary>
	void SetHit(const AttackData& data);

private:

private:
	CharacterParameterComponent* paremeter;
	ObjectComponent* object;
	std::vector<AttackData> datas_;
};

/// <summary>
/// 衝突応答
/// </summary>
class HitResponse {
public:

	// 衝突応答(タグごとの)
	void Hit(CollisionTag tag,Collider* self, Collider* other);

	// 使っているもののワールドトランスフォームを設定
	void SetOwner(WorldTransform* owner) { transform = owner; };

private:
	WorldTransform* transform;	//　衝突応答用

};

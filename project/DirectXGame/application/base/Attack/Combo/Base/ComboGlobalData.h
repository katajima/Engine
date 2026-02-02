#pragma once
#include <DirectXGame/engine/struct/Structs3D.h>


enum class HitBoxSpawnType {
	kOnTime,			// 時間で発生
	kOnGround,			// 着地で発生
	kOnAir,				// 空中から発生
	kOnButtonRelease,	// ボタンを離したら
};

enum class  HitBoxParentType {
	kParent,			// 追従型
	kIndependent,		// 独立(飛び道具や範囲攻撃など)
	kParentIndependent,	// 追従位置での独立(武器の中間位置で発生してその後独立)

	kLockOnArea,		// ターゲット位置
};

// 終了条件タイプ
enum class EndConditionType {
	kOnGround,			// 着地したら
	kOnButtonRelease,	// ボタンを離したら
	kOnMeterEmpty,		// メータが空になったら
	kOnTimer,			// 時間が過ぎたら
	kOnHit,				// 何かに当たったら
	kManual,			// 特殊ケース
};

// 保存項目用コンボデータ
struct ComboGlovalData {

	std::string animationName = "";	// アニメーション名


	float hitBoxWindowStart_ = 0.0f;	// ヒットボックスの生成タイミング時間
	float hitBoxLifeTime_ = 0.5f;		// ヒットボックスの生存時間
	Vector3 offsetPos_ = {};			// コライダー位置
	Vector3 colliderSize = { 1.0f,1.0f,1.0f };	// コライダーサイズ

	// 敵に送るリアクションデータ
	struct Reaction {
		float knockbackDuration_ = 0.1f;	// ノックバック持続時間
		float knockbackPower = 30.0f;		// ノックバックパワー
		float knockbackPowerY = 30.0f;		// ノックバックY方向パワー
		bool isVerticalBoost_ = false;		// Y方向にノックバックするか

		float damage = 0;					// ダメージ
	};
	// リアクションデータ
	Reaction reaction;


	// 移動関係
	float moveSpeed_ = 1.0;				// 移動速度
	float moveWindowStart_ = 0.0f;		// 移動開始
	float moveWindowEnd_ = 1.0f;		// 移動終了
	bool  isCompulsionMove_ = true;		// 強制移動



	float stateInputStartTime = 0.1f;	// 入力受付時間
	float stateInputEndTime = 0.5f;		// 入力終了時間

	float stateCancelStartTime = 0.3f;	// キャンセル開始時間
	float stateCancelEndTime = 0.5f;	// キャンセル終了時間

	float stateEndTime = 0.5f;			// ステート終了時間
	float stateNextTime = 0.45f;		// ステート移行時間


	bool isGravity = true;				// 重力はあるか？
	float gravityScale = 1.0f;			// 重力スケール

	float animationSpeed_ = 1.0f;		// アニメーションスピード
	float animationBlendTime_ = 0.1f;	// アニメーションブレンド時間

	float trailEffectStartTime = 0.1f;	// トレイル発生時間
	float trailEffectLifeTime = 1.0;	// トレイル生存時間

	/// 親子付け設定  ///　 
	std::string parentName_ = "なし";
	Vector3 parentOffset_ = { 0.0f,0.0f,0.0f };

	

	// ヒットボックスの発生条件タイプ
	HitBoxSpawnType spawnType_ = HitBoxSpawnType::kOnTime;
	// ヒットボックス依存先タイプ
	HitBoxParentType dependenceType_ = HitBoxParentType::kParent;


	///	終了条件 ///
	EndConditionType endConditionType = EndConditionType::kOnTimer;

};
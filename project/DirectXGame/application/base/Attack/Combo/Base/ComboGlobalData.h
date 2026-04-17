#pragma once
#include "DirectXGame/application/base/Attack/HitBox/HitBoxData.h"
#include "DirectXGame/application/base/Attack/LockOn/LockOnData.h"
#include "DirectXGame/application/base/Attack/AttackData.h"

namespace Combo {

	// 終了条件タイプ
	enum class EndConditionType {
		kOnGround,			// 着地したら
		kOnButtonRelease,	// ボタンを離したら
		kOnMeterEmpty,		// メータが空になったら
		kOnTimer,			// 時間が過ぎたら
		kOnHit,				// 何かに当たったら
		kManual,			// 特殊ケース
	};

	// 移動方法
	enum class MoveType {
		kNone,		// 特になし
		kTraget,	// ターゲットに向かって
		kForward,	// 前方
		kLockAt,	// カメラ方向
	};

	// コンボタイプ
	enum class Type{
		kNone,		// なし
		kMelle,		// 近距離
		kRange,		// 遠距離
		kMix,		// 合わせ
	};


	// 保存項目移動データ
	struct GlobalMove {
		// 移動速度
		float moveSpeed = 1.0;			
		// 移動開始
		float moveWindowStart = 0.0f;	
		// 移動終了
		float moveWindowEnd = 1.0f;		
		// 強制移動
		bool  isCompulsionMove = true;		
		
		// 重力はあるか？
		bool isGravity = true;				
		// 重力スケール
		float gravityScale = 1.0f;			
		// 移動方向条件
		MoveType moveType = MoveType::kForward;

	};

	// 保存項目ヒットボックス
	struct GlobalHitBox {
		// ヒットボックスの生成タイミング時間
		float hitBoxWindowStart = 0.0f;	
		// ヒットボックスの生存時間
		float hitBoxLifeTime = 0.5f;
		// ヒット記録を使用するか（使用した場合連続ヒットしない）
		bool useContactRecord = true;
		// コライダー位置
		Vector3 offsetPos = {};			
		// コライダーサイズ
		Vector3 colliderSize = { 1.0f,1.0f,1.0f };	
		/// 親子付け設定  ///　 
		std::string parentName = "なし";
		Vector3 parentOffset = { 0.0f,0.0f,0.0f };
		// ヒットボックスの発生条件タイプ
		HitBox::SpawnType spawnType = HitBox::SpawnType::kOnTime;
		// ヒットボックス依存先タイプ
		HitBox::ParentType dependenceType = HitBox::ParentType::kParent;
		// ヒットボックス生存タイプ
		HitBox::LifetimeType lifetimeType = HitBox::LifetimeType::kTimed;
		// ヒットボックス影響タイプ
		HitBox::HitEffectType hitEffectType = HitBox::HitEffectType::kDamageAndForce;
	};

	// 保存項目リアクション
	struct GlobalHitReaction {
		// ヒットリアクションタイプ
		HitReactionType hitReactionType = HitReactionType::Knockback;
		// 水平方向の強さ
		float power = 0.0f;
		// 垂直方向の強さ
		float verticalBoost = 0.0f;
		// 上方向を強制するか
		bool isVerticalBoost = false;
		// リアクション移動の有効時間
		float duration = 0.25f;
		// 行動不能時間
		float hitStunTime = 0.1f;
		// ダウン時間
		float downTime = 0.0f;
		// 打ち上げ時に重力を弱める/止める時間
		float launchFloatTime = 0.0f;
		// 重力を適用するか
		bool gravityEnabled = false;	
		// 重力倍率
		float gravityScale = 1.0f;
		// ダメージ
		float damage = 0;					
	};

	// 保存項目ロックオン
	struct GlobalLockOn {
		// ロックオンタイプ
		LockOnType lockOnType = LockOnType::kHit;
		// ロックオン半径
		float lockOnRadius = 7.0f;
		// ターゲットにどこまで近づくか半径
		float moveTragetRadius = 1.0f;
	};

	// 保存項目アニメーション
	struct GlobalAnimation {
		// アニメーション名
		std::string animationName = "";	
		// アニメーションスピード
		float animationSpeed = 1.0f;		
		// アニメーションブレンド時間
		float animationBlendTime = 0.1f;	

	};

	// 保存項目用コンボデータ
	struct GlobalData {

		Type type = Type::kMelle;	// 攻撃タイプ

		// 敵に送るリアクションデータ
		GlobalHitReaction hitReaction{};
		// 移動関係
		GlobalMove move{};
		// ヒットボックス関係
		GlobalHitBox hitBox{};
		// ロックオン
		GlobalLockOn lockOn{};
		// アニメーション
		GlobalAnimation animation{};
	
		float stateInputStartTime = 0.1f;	// 入力受付時間
		float stateInputEndTime = 0.5f;		// 入力終了時間

		float stateCancelStartTime = 0.3f;	// キャンセル開始時間
		float stateCancelEndTime = 0.5f;	// キャンセル終了時間

		float stateMoveCancelStartTime = 0.3f;	// キャンセル開始時間(移動)
		float stateMoveCancelEndTime = 0.5f;	// キャンセル終了時間(移動)

		float stateEndTime = 0.5f;			// ステート終了時間
		float stateNextTime = 0.45f;		// ステート移行時間

		float trailEffectStartTime = 0.1f;	// トレイル発生時間
		float trailEffectLifeTime = 1.0;	// トレイル生存時間

	
		///	終了条件 ///
		EndConditionType endConditionType = EndConditionType::kOnTimer;
	};
};

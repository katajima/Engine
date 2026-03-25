#pragma once
#include "DirectXGame/application/base/Attack/HitBox/HitBoxData.h"
#include "DirectXGame/application/base/Attack/LockOn/LockOnData.h"

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
		float moveSpeed_ = 1.0;			
		// 移動開始
		float moveWindowStart_ = 0.0f;	
		// 移動終了
		float moveWindowEnd_ = 1.0f;		
		// 強制移動
		bool  isCompulsionMove_ = true;		
		
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
		float hitBoxWindowStart_ = 0.0f;	
		// ヒットボックスの生存時間
		float hitBoxLifeTime_ = 0.5f;		
		// コライダー位置
		Vector3 offsetPos_ = {};			
		// コライダーサイズ
		Vector3 colliderSize = { 1.0f,1.0f,1.0f };	
		/// 親子付け設定  ///　 
		std::string parentName_ = "なし";
		Vector3 parentOffset_ = { 0.0f,0.0f,0.0f };
		// ヒットボックスの発生条件タイプ
		HitBox::SpawnType spawnType_ = HitBox::SpawnType::kOnTime;
		// ヒットボックス依存先タイプ
		HitBox::ParentType dependenceType_ = HitBox::ParentType::kParent;
	};

	// 保存項目リアクション
	struct GlobalReaction {
		// ノックバック持続時間
		float knockbackDuration_ = 0.1f;	
		// ノックバックパワー
		float knockbackPower = 30.0f;		
		// ノックバックY方向パワー
		float knockbackPowerY = 30.0f;		
		// Y方向にノックバックするか
		bool isVerticalBoost_ = false;		
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
		float animationSpeed_ = 1.0f;		
		// アニメーションブレンド時間
		float animationBlendTime_ = 0.1f;	

	};

	// 保存項目用コンボデータ
	struct GlobalData {

		Type type = Type::kMelle;	// 攻撃タイプ

		// 敵に送るリアクションデータ
		GlobalReaction reaction{};
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

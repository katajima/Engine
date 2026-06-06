#pragma once
#include "DirectXGame/application/base/Attack/HitBox/HitBoxData.h"
#include "DirectXGame/application/base/Attack/LockOn/LockOnData.h"
#include "DirectXGame/application/base/Attack/AttackData.h"
#include "DirectXGame/application/base/Camera/Base/CameraData.h"

namespace Character {
	struct CharacterContext;
	class BaseCharacter;
}

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
		kInput,		// 入力方向基準
		kTraget,	// ターゲット方向基準
		kForward,	// 自キャラ前方基準
		kLockAt,	// カメラ方向基準
	};


	// コンボタイプ
	enum class Type {
		kNone,		// なし
		kMelle,		// 近距離
		kRange,		// 遠距離
		kMix,		// 合わせ
	};

	// 遠距離タイプ
	enum class RangeType {
		kBullet,	// 弾
		kWeapon,	// 武器
		kSubWeapon,	// サブ武器
	};

	// 保存項目用遠距離攻撃データ
	struct GlobalRange {
		RangeType rangeType = RangeType::kBullet;	// 遠距離処理で使用する攻撃物
		float rangeWindowStart = 0.1f;
		float rangeWindowEnd = 0.5f;
		float speed = 1.0f;
		float interval = 0.5f;
		int count = 1;
		float damage = 1.0f;
		Vector3 subWeaponIdleOffset = { 0.6f, 1.1f, 0.2f };	// サブ武器の待機位置
		Vector3 subWeaponStartOffset = { 0.0f, 1.1f, 0.0f };	// 投擲開始位置のオフセット
		float subWeaponThrowSpeed = 28.0f;					// サブ武器の投擲速度
		float subWeaponThrowLifeTime = 0.35f;				// サブ武器の投擲時間
		float subWeaponReturnTime = 0.18f;					// サブ武器の戻り時間
		float subWeaponSpinSpeed = 18.0f;					// サブ武器の回転速度
	};

	// 攻撃ノード単位のゲームプレイ調整項目。
	struct GlobalAction {
		// 個別コストを使うか。falseなら従来どおり入力種別側のコストを使う。
		bool useCustomStaminaCost = false;
		float staminaCost = 0.0f;
		float cooldown = 0.0f;
		int requiredAirRemainCount = 0;

		// 攻撃中に一時的に付与したい防御/割り込み耐性。
		bool superArmor = false;
		bool invincible = false;
		bool guardPoint = false;

		// キャンセル分岐の制約。
		bool cancelOnHitOnly = false;
		bool cancelOnMissOnly = false;
		bool landingCancel = false;

		// 攻撃演出の追加フック。
		float hitPauseScale = 1.0f;
		float cameraShakePower = 0.0f;
		std::string soundName = "";
	};
	// 時間
	struct StateTime {
		// 入力受付時間
		float startTime = 0.1f;
		// 入力終了時間
		float endTime = 0.5f;
	};

	// 保存項目移動データ
	struct GlobalMove {
		// 受付時間
		StateTime moveWindow{};
		// 移動速度（軸ごと）
		// X=Right Y=Up Z=Forward
		Vector3 moveSpeed = { 0.0f, 0.0f, 0.0f };
		// 強制的に移動
		bool isCompulsionMove = true;
		// 空中でのコンボで重力はあるか？
		bool isGravity = true;
		// 重力スケール
		float gravityScale = 1.0f;
		// 攻撃中の最大落下速度。0以下なら制限なし。
		float maxFallSpeed = 0.0f;
		// 開始時に重力速度をリセットするか
		bool isResetGravity = false;
		
		// ローカル移動ベクトル
		// X=Right, Y=Up, Z=Forward
		// 例:
		// { 0, 0, 1 }  : 前進
		// { 0, 0,-1 }  : 後退
		// { 1, 0, 0 }  : 右移動
		// { 0, 1, 0 }  : 上昇
		// { 0, 1, 1 }  : 前上方向
		Vector3 localMoveVector = { 0.0f, 0.0f, 1.0f };

		// localMoveVector を正規化してから使うか
		bool isNormalizeLocalMove = true;

		// 移動中も毎フレーム方向を更新するか
		// ターゲット追尾や入力追従系で有効
		bool isUpdateDirectionEachFrame = true;

		// ターゲット方向を使うとき、基準前方を水平化するか
		// trueなら従来通り地面基準に近い挙動
		bool isFlattenTargetDirection = true;

		// 移動方向とキャラクターの向く方向を一致させるか？
		bool alignCharacterToMovement = true;

		// ターゲット方向を向くか
		bool isTargetDirection = false;

		// 移動タイプ
		MoveType moveType = MoveType::kTraget;
		// ロックオンタイプ
		LockOnData lockOnData{};
	};

	// 保存項目アニメーション
	struct GlobalAnimation {
		// アニメーション名
		std::string animationName = "";
		// アニメーションスピード
		float animationSpeed = 1.0f;
		// アニメーションブレンド時間
		float animationBlendTime = 0.1f;
		// アニメーションループ
		bool animationLoop = false;
		// アニメーションを一定の場所で止めるか？
		bool animationStop = false;
		// アニメーションの止めるタイミング
		float animationStopTime = 1.0f;
	};

	// 保存項目条件データ
	struct GlobalCondition {
		// 入力受付
		StateTime stateInput{};
		// 先行入力として保持する寿命
		float inputBufferTime = 0.25f;
		// 入力受付から次のステートに移行するまでの時間
		float inputDelay = 0.0f;
		// キャンセル受付
		StateTime stateCancel{};
		// 移動キャンセル受付
		StateTime stateMoveCancel{};
		// キャンセル可能か
		bool isCancel = true;
		// キャンセル可能かどうか(スティック移動での)
		bool isMoveCancel = true;
		// ステート終了時間
		float stateEndTime = 0.5f;
		// ステート移行時間
		float stateNextTime = 0.45f;
		// 強制的に次のコンボに移行するか 
		bool isCompulsionNext = false;
		///	終了条件 ///
		EndConditionType endConditionType = EndConditionType::kOnTimer;
	};

	// 保存項目エフェクトデータ
	struct GloblEffectData {
		// トレイル発生時間
		float trailEffectStartTime = 0.1f;
		// トレイル生存時間
		float trailEffectLifeTime = 1.0;
	};

	// 保存項目用コンボ接続データ
	struct GlobalConditionalConnection {
		std::string groundMiss = "";
		std::string groundHit = "";
		std::string airMiss = "";
		std::string airHit = "";
	};

	struct GlobalConnection {
		std::string lightAttack = "";
		std::string heavyAttack = "";
		std::string skill = "";
		GlobalConditionalConnection lightCondition{};
		GlobalConditionalConnection heavyCondition{};
		GlobalConditionalConnection skillCondition{};
	};

	// 保存項目用コンボデータ
	struct GlobalData {
		// 攻撃ノード単位の調整項目
		GlobalAction action{};
		// 攻撃タイプ
		Type type = Type::kMelle;
		// 遠距離攻撃
		GlobalRange range{};
		// 敵に送るリアクションデータ
		HitReactionData hitReaction{};
		// ヒットボックス関係
		HitBox::GlobalHitBox hitBox{};
		// 移動関係
		GlobalMove move{};
		// アニメーション
		GlobalAnimation animation{};
		// 条件
		GlobalCondition condition{};
		// エフェクト
		GloblEffectData effect{};
		// カメラ 
		GlobalCameraData camera{};
		// 次のコンボへの接続
		GlobalConnection connection{};
	};
};

#pragma once
#include "DirectXGame/application/base/Attack/HitBox/HitBoxData.h"
#include "DirectXGame/application/base/Attack/LockOn/LockOnData.h"
#include "DirectXGame/application/base/Attack/AttackData.h"
#include "DirectXGame/application/base/Camera/Base/CameraData.h"
#include <string>
#include <vector>

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

	// 攻撃移動の速度カーブ
	enum class MoveSpeedCurveType {
		kConstant,		// 一定速度
		kEaseIn,		// 徐々に加速
		kEaseOut,		// 徐々に減速
		kEaseInOut,		// 加速してから減速
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

	// 遠距離ロックオンタイプ
	enum class RangeLockOnType {
		kNone,		// ロックオンなし(向いている方向)
		kTarget,	// ターゲットロックオン
		kCamera,	// カメラロックオン
		kOffsetTarget,	// 自分基準のオフセット位置
	};

	// オフセットターゲットの座標タイプ
	enum class RangeOffsetTargetType {
		kWorldOffset,		// 所有者位置にワールド軸のオフセットを足す
		kOwnerLocalOffset,	// 所有者の向き基準のローカルオフセットを足す
	};

	// 投擲物の移動タイプ
	enum class RangeThrowMoveType {
		kStraight,			// 直進
		kStopOnHit,			// ヒット時にその場で停止
		kTeleportToTarget,	// ターゲット位置へ瞬間移動
		kLerpToTarget,		// ターゲット位置へ補間移動
	};

	// 投擲物の効果発動条件
	enum class RangeEffectTriggerType {
		kNone,		// 効果発動なし
		kButton,	// ボタン入力で発動
		kTimer,		// 時間経過で発動
		kHit,		// ヒットで発動
	};

	// 投擲物の回収条件
	enum class RangeRecallTriggerType {
		kNone,		// 自動回収なし
		kButton,	// ボタン入力で回収
		kTimer,		// 時間経過で回収
		kNearOwner,	// 投擲物に近づいたら回収
	};

	// コンボ演出の発生条件
	enum class ComboEffectTriggerType {
		kTimeWindow,	// 指定時間範囲中に頻度ごと発生
		kTimer,			// 指定時間経過で一回発生
		kLanding,		// 着地した瞬間に一回発生
	};

	// 保存項目用遠距離攻撃データ
	struct GlobalRange {
		RangeType rangeType = RangeType::kBullet;	// 遠距離処理で使用する攻撃物
		RangeLockOnType lockOnType = RangeLockOnType::kTarget;	// 投擲や弾の狙い方
		RangeOffsetTargetType offsetTargetType = RangeOffsetTargetType::kWorldOffset;	// オフセットターゲットの座標タイプ
		float lockOnStartRadius = 30.0f;						// ターゲットを狙い始める半径
		Vector3 offsetTarget = { 0.0f, 0.0f, 20.0f };			// 狙うオフセット位置
		RangeThrowMoveType throwMoveType = RangeThrowMoveType::kStraight;	// 投擲物の移動タイプ
		RangeEffectTriggerType effectTriggerType = RangeEffectTriggerType::kNone;	// 効果発動条件
		RangeRecallTriggerType recallTriggerType = RangeRecallTriggerType::kTimer;	// 回収条件
		float throwLerpTime = 0.25f;					// ターゲット位置へ補間移動する時間
		float effectTriggerTime = 0.35f;				// 時間経過で効果発動するまでの時間
		float recallTriggerTime = 0.8f;				// 時間経過で回収するまでの時間
		float recallNearRadius = 2.0f;				// 近づいたら回収する半径
		float throwStayTime = 0.0f;					// 投げてから回収までその場に留まる時間
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
		bool subWeaponAlignToDirection = true;				// 投擲方向へ向きを合わせるか
		bool subWeaponUseSpin = true;						// 投擲中に回転させるか
		Vector3 subWeaponRotateOffset = { 1.5708f, 0.0f, 3.1416f };	// サブ武器の回転オフセット
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
	struct GlobalMoveTargetParameters {
		bool enabled = false;							// ターゲット有無別の移動設定を使うか
		Vector3 moveSpeed = { 0.0f, 0.0f, 0.0f };		// 条件一致時の移動速度
		Vector3 localMoveVector = { 0.0f, 0.0f, 1.0f };	// 条件一致時のローカル移動方向
		bool isNormalizeLocalMove = true;				// 条件一致時のローカル移動方向を正規化するか
		TargetMoveType targetMoveType = TargetMoveType::kMove;	// ターゲットあり時の接近処理
		float moveTargetRadius = 1.0f;					// ターゲットあり時の接近停止半径
	};

	struct GlobalMove {
		// 攻撃開始直前の通常移動速度をコンボ中も維持するか
		bool inheritMoveInertia = false;
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

		// ターゲット位置も毎フレーム更新するか
		// 動く敵への吸い付きや接近技で有効
		bool isUpdateTargetPositionEachFrame = true;

		// 攻撃移動の速度カーブ
		MoveSpeedCurveType speedCurveType = MoveSpeedCurveType::kConstant;

		// 速度カーブの強さ
		float speedCurvePower = 1.0f;

		// 縦方向の攻撃移動を許可するか
		bool isVerticalMove = true;

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
		// ターゲットがいる時だけ使う移動上書き
		GlobalMoveTargetParameters targetMove{};
		// ターゲットがいない時だけ使う移動上書き
		GlobalMoveTargetParameters noTargetMove{};
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


		// アニメーションのルートモーションを使うか？
		bool isAnimationRootMotion = false;

		// コンボ中にキャラクター本体のTransformアニメーションを使用するか
		bool isTransformAnimation = false;
		// Transformアニメーションの開始時間
		float transformStartTime = 0.0f;
		// Transformアニメーションの終了時間
		float transformEndTime = 0.5f;
		// 開始時に加算するTransformオフセット
		Transform transformStart = { {}, {}, {} };
		// 終了時に加算するTransformオフセット
		Transform transformEnd = { {}, {}, {} };
		// コンボ終了時に適用中のTransformオフセットを取り除くか
		bool restoreTransformOnExit = true;

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

	// コンボ中に指定時間で発生させるエフェクト
	struct ComboEffectEntry {
		std::string effectName = "";					// 発生させるエフェクト名
		std::string parentName = "Player";				// 発生位置の追従先
		ComboEffectTriggerType triggerType = ComboEffectTriggerType::kTimeWindow;	// 発生条件
		float startTime = 0.0f;							// コンボ開始から発生開始までの時間
		float endTime = 0.0f;							// コンボ開始から発生終了までの時間
		float interval = 0.1f;							// 発生頻度
		Vector3 offset = { 0.0f, 0.0f, 0.0f };			// 追従先からの発生オフセット
	};

	// 保存項目エフェクトデータ
	struct GloblEffectData {
		// トレイル発生時間
		float trailEffectStartTime = 0.1f;
		// トレイル生存時間
		float trailEffectLifeTime = 1.0;
		// コンボ中に武器を表示するか
		bool weaponDraw = true;
		// コンボ中に一度だけ発生させるエフェクト一覧
		std::vector<ComboEffectEntry> comboEffects;
	};

	// 保存項目用コンボ接続データ
	struct GlobalConditionalConnection {
		std::string groundMiss = "";
		std::string groundHit = "";
		std::string airMiss = "";
		std::string airHit = "";
		std::string lockOn = "";
		std::string noLockOn = "";
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

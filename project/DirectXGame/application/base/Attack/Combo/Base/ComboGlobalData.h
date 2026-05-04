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
		kInput,		// 入力方向基準
		kTraget,	// ターゲット方向基準
		kForward,	// 自キャラ前方基準
		kLockAt,	// カメラ方向基準
	};

	// ターゲットまでの移動タイプ
	enum class TargetMoveType {
		kNone,		// なし
		kMove,		// 移動
		kTeleport,	// 瞬間移動
	};

	// コンボタイプ
	enum class Type {
		kNone,		// なし
		kMelle,		// 近距離
		kRange,		// 遠距離
		kMix,		// 合わせ
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
		// 開始時に重力速度をリセットするか
		bool isResetGravity = false;
		// ターゲットの距離でどこまで近づくか
		float moveTargetRadius = 1.0f;
		// ターゲットまで移動するときの移動タイプ
		TargetMoveType targetMoveType = TargetMoveType::kMove;

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
		// 入力受付から次のステートに移行するまでの時間(複数入力猶予に使う)
		float stateInputToNextTime = 0.0f;
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

	// 保存項目用カメラデータ
	struct GlobalCamera {
		// 使用カメラ
		std::string cameraName = "no";
		// 元のキャラクターに使用しているカメラ名
		std::string baseCameraName = "no";
		// カメラを変更するか
		bool isChangeCamera = false;
		// 移動補間
		float interpolation = 0.0f;
		// 
		float shakeCameraPower;
		//
		float shakeCameraTime;
		// 
		float zoomCameraAmount;
	};

	// 保存項目エフェクトデータ
	struct GloblEffectData {
		// トレイル発生時間
		float trailEffectStartTime = 0.1f;
		// トレイル生存時間
		float trailEffectLifeTime = 1.0;
	};

	// 保存項目用コンボデータ
	struct GlobalData {
		// 攻撃タイプ
		Type type = Type::kMelle;
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
	};
};

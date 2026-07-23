#pragma once
// C++
#include <optional>
#include <memory>

// 自作
#include <DirectXGame/engine/struct/Structs.h>
#include "DirectXGame/engine/Utility/ConvertUtility.h"

// 前方宣言
class EffectSystem;
class BaseSpecial;
class BaseWeapon;
class BulletManager;
class CameraManager;
class SpecialPointManager;
namespace Engine {
	class EntityManager;
}

// キャラクター
namespace Character {
	// キャラクターの種類を定義する列挙型
	enum class Type
	{
		None,		// キャラクターなし
		Player,		// プレイヤーキャラクター
		Enemy,		// 敵キャラクター
		NPC,		// NPCキャラクター
		Boss,		// ボスキャラクター
		Friendly,	// フレンドリーキャラクター
	};

	// キャラクターの基本パラメータを定義する構造体
	struct BasicParameters
	{
		Gage HP;				// ヒットポイント
		Gage MP;				// マジックポイント
		Gage stamina;			// スタミナ
		float strength = 1.0f;	// 力
		float defense = 1.0f;	// 防御力
	};


	// キャラクターの性格を定義する列挙型
	enum class Personality
	{
		kNormal,			// 特になし
		kAggressive,		// 攻撃的
		kDefensive,			// 防御的
		kCautious,			// 用心深い
		kCurious,			// 好奇心旺盛
		kFriendly,			// フレンドリー
		kObsession			// 執着的
	};

	// 基本的な振るまい
	enum class BasicBehavior {
		kRoot,			// 通常状態
		kAttack,		// 攻撃中
		kDie,			// 死亡状態
		kSpecialAttack,	// 特殊攻撃中
	};

	// 通常時の振るまい
	enum class RootBehavior {
		kIdle,		// 待機状態
		kMove,		// 移動状態
		kJump,		// ジャンプ状態
	};

	// プレイヤーキャラクターの種類を定義する列挙型
	enum class PlayerType
	{
		kNormal,	// 通常
		kBullet,	// 弾撃ち
	};




	// キャラクターパラメータコンポーネント
/// <summary>
/// ParameterComponentを管理・実装するクラス。
/// </summary>
	class ParameterComponent
	{
	public:
		void Initialize() {
			parameters = std::make_unique<BasicParameters>();
		}


		/// <summary>
		/// 更新
		/// </summary>
		void Update() {
			parameters->HP.Update(deltaTime_);	// HP更新
			parameters->MP.Update(deltaTime_);	// MP更新
			parameters->stamina.Update(deltaTime_);	// スタミナ更新
		}


	public:
		/// <summary>
		/// HP取得
		/// </summary>
		float GetHP() const { return parameters->HP.value; }
		/// <summary>
		/// MP取得
		/// </summary>
		float GetMP() const { return parameters->MP.value; }
		/// <summary>
		/// スタミナ取得
		/// </summary>
		float GetStamina() const { return parameters->stamina.value; }
		/// <summary>
		/// パワー取得
		/// </summary>
		float GetStrength() const { return parameters->strength; }
		/// <summary>
		/// 防御力取得
		/// </summary>
		float GetDefense() const { return parameters->defense; }

		/// <summary>
		/// スタミナがあるか
		/// </summary>
		bool IsGetStamina() const {
			return !parameters->stamina.IsEmpty();
		}

		//

		/// <summary>
		/// HP取得
		/// </summary>
		Gage& HP() const { return parameters->HP; };
		/// <summary>
		/// MP取得
		/// </summary>
		Gage& MP() const { return  parameters->MP; };
		/// <summary>
		/// スタミナ取得
		/// </summary>
		Gage& Stamina() const { return parameters->stamina; };



		/// <summary>
		/// 性格取得
		/// </summary>
		Personality GetPersonality() const { return personality; }
		/// <summary>
		/// キャラクター種類
		/// </summary>
		Type GetCharacterType() const { return characterType_; }
	public:
		Personality personality = Personality::kNormal;
		Type characterType_ = Type::None;							// キャラクターの種類
		std::unique_ptr<BasicParameters> parameters;				// 基本パラメータ

		float deltaTime_ = ConvertUtility::kDefaultDeltaTime;	// パラメータ回復用の基準デルタタイム

	};
}






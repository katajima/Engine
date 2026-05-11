#pragma once
// C++
#include <optional>
#include <memory>

// 自作
#include <DirectXGame/engine/struct/Structs.h>

// 前方宣言
class EffectSystem;
class BaseSpecial;
class BaseWeapon;
class BulletManager;
class CameraManager;
class SpecalPointManager;
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
	class ParameterComponent
	{
	public:
		void Initialize() {
			parameters = std::make_unique<BasicParameters>();
		}


		// 更新
		void Update() {
			parameters->HP.Update(deltaTime_);	// HP更新
			parameters->MP.Update(deltaTime_);	// MP更新
			parameters->stamina.Update(deltaTime_);	// スタミナ更新
		}


	public:
		// HP取得
		float GetHP() const { return parameters->HP.value; }
		// MP取得
		float GetMP() const { return parameters->MP.value; }
		// スタミナ取得
		float GetStamina() const { return parameters->stamina.value; }
		// パワー取得
		float GetStrength() const { return parameters->strength; }
		// 防御力取得
		float GetDefense() const { return parameters->defense; }
		
		// スタミナがあるか
		bool IsGetStamina() const {
			return !parameters->stamina.IsEmpty();
		}

		// 

		// HP取得
		Gage& HP() const { return parameters->HP; };
		// MP取得
		Gage& MP() const { return  parameters->MP; };
		// スタミナ取得
		Gage& Stamina() const { return parameters->stamina; };



		// 性格取得
		Personality GetPersonality() const { return personality; }
		// キャラクター種類
		Type GetCharacterType() const { return characterType_; }
	public:
		Personality personality = Personality::kNormal;
		Type characterType_ = Type::None;							// キャラクターの種類
		std::unique_ptr<BasicParameters> parameters;				// 基本パラメータ

		float deltaTime_ = 1.0f / 60.0f;

	};
}






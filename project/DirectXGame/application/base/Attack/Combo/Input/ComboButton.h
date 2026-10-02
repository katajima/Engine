#pragma once
#include <DirectXGame/engine/input/Input.h>

#include "vector"


class InputSystem;
namespace Character {
	struct CharacterContext;
	class BaseCharacter;
}

namespace Combo {
	// コンボ条件が参照する入力の種類です。
	enum class ComboInputSource : uint8_t {
		GamePadButton,
		CharacterAction,
	};

	// キャラクター入力として扱えるアクションです。
	enum class ComboActionType : uint8_t {
		LightAttack,
		HeavyAttack,
		Skill,
		Jump,
		Dodge,
		Guard,
		Special,
	};

	// 入力方向の条件です。Anyなら方向を評価しません。
	enum class ComboInputDirection : uint8_t {
		Any,
		Neutral,
		Up,
		Down,
		Left,
		Right,
	};

	/// <summary>
	/// コンボ条件ボタン
	/// </summary>
	enum class ComboButtonInputType : uint32_t {
		kPressed,				// 押したら
		kTriggered,				// 押した瞬間
		kReleased,				// 離した瞬間
		kPressTriggerReleased,	// 押したor押した瞬間or離した瞬間
		kPressTriggered,		// 押したor押した瞬間
		kPressReleased,			// 押したor離した瞬間
		kTriggerReleased,		// 押した瞬間or離した瞬間
	};


	// コンボデータ上で扱うゲームパッドボタン種別
	enum class ComboGamePadButton {
		GAMEPAD_Up = 0,			// 十字(上)
		GAMEPAD_Down = 1,		// 十字(下)
		GAMEPAD_Left = 2,		// 十字(左)
		GAMEPAD_Right = 3,		// 十字(右)
		GAMEPAD_A = 4,			// A
		GAMEPAD_B = 5,			// B
		GAMEPAD_X = 6,			// X
		GAMEPAD_Y = 7,			// Y
		GAMEPAD_LB = 8,			// LB
		GAMEPAD_RB = 9,			// RB
		GAMEPAD_LS = 10,			// 左スティック押し込み
		GAMEPAD_RS = 11,			// 右スティック押し込み
		GAMEPAD_Start = 12,			// Start
		GAMEPAD_Back = 13,				// Back
		GAMEPAD_Max       // 最大ボタン数
	};

	/// <summary>入力ソース、方向、長押し時間をまとめた1ステップの設定です。</summary>
	struct ComboInputStep {
		ComboInputSource source = ComboInputSource::GamePadButton;
		ComboGamePadButton button = ComboGamePadButton::GAMEPAD_B;
		ComboActionType action = ComboActionType::LightAttack;
		ComboButtonInputType inputType = ComboButtonInputType::kPressed;
		ComboInputDirection direction = ComboInputDirection::Any;
		float minHoldSeconds = 0.0f;
		float maxHoldSeconds = 0.0f;
		float minChargeRatio = 0.0f;
		float maxChargeRatio = 1.0f;
		float chargeSeconds = 1.0f;
	};

	/// <summary>
	/// コンボ用ボタン種別を入力システム用ボタン種別へ変換する
	/// </summary>
	GamePadButton ConvertGamePadButton(ComboGamePadButton button);

	/// <summary>
	/// コンボボタン1つ分
	/// </summary>
	class ComboButton {
	public:
		/// <summary>
		/// コンストラクタ
		/// </summary>
		ComboButton(ComboGamePadButton button, ComboButtonInputType type) : button_(button), type_(type) {}
		/// <summary>拡張入力ステップからコンボボタンを作成します。</summary>
		/// <param name="step">入力ソース、方向、保持時間を含む設定です。</param>
		explicit ComboButton(const ComboInputStep& step);

		/// <summary>
		/// 押したら
		/// </summary>
		bool IsPressed(const InputSystem& inputSystem) const;

		/// <summary>
		/// 押した瞬間
		/// </summary>
		bool IsTriggered(const InputSystem& inputSystem) const;

		/// <summary>
		/// 離した瞬間
		/// </summary>
		bool IsReleased(const InputSystem& inputSystem) const;


			/// <summary>
			/// 押して反応する条件
			/// </summary>
		bool IsInput(const InputSystem& inputSystem) const;
		/// <summary>キャラクターコンテキストを使って拡張入力を評価します。</summary>
		/// <param name="ctx">入力データ、方向、経過時間を含むコンテキストです。</param>
		bool IsInput(const Character::CharacterContext& ctx) const;

		/// <summary>
		/// どのボタンに反応するかを設定する
		/// </summary>
		void SetGamePadButton(ComboGamePadButton button) { button_ = button; };
		/// <summary>入力ステップの設定を上書きします。</summary>
		/// <param name="step">適用する入力設定です。</param>
		void SetStep(const ComboInputStep& step);
		/// <summary>入力状態を新しいシーケンス開始状態へ戻します。</summary>
		void ResetState() const;

	private:
		ComboGamePadButton button_;	// 判定対象のゲームパッドボタン
		ComboButtonInputType type_ = ComboButtonInputType::kPressed;	// 押下、トリガー、リリースなどの入力条件
		ComboInputSource source_ = ComboInputSource::GamePadButton;	// 入力を取得する場所
		ComboActionType action_ = ComboActionType::LightAttack;	// キャラクターアクションの種類
		ComboInputDirection direction_ = ComboInputDirection::Any;	// 必要な方向
		float minHoldSeconds_ = 0.0f;	// 最小保持時間
		float maxHoldSeconds_ = 0.0f;	// 最大保持時間。0は無制限
		float minChargeRatio_ = 0.0f;	// 最小溜め率
		float maxChargeRatio_ = 1.0f;	// 最大溜め率
		float chargeSeconds_ = 1.0f;	// 最大溜め時間
		mutable float holdSeconds_ = 0.0f;	// 現在の保持時間
		mutable float releasedHoldSeconds_ = 0.0f;	// 直前の離し時保持時間
		mutable bool wasPressed_ = false;	// 前フレームの押下状態
	};


	/// <summary>
	/// コンボ（ボタンの順番を管理）
	/// </summary>
	class ComboSequence {
	public:
		/// <summary>
		/// コンボボタンを順番に登録
		/// </summary>
		void RegisterCombo(const std::vector<ComboButton>& buttons, bool sequential = false);

		/// <summary>
		/// コンボ成立チェック
		/// </summary>
		bool Update(const Character::CharacterContext& ctx);
		/// <summary>現在のシーケンス入力位置を取得します。</summary>
		/// <returns>次に成立させる入力のインデックスです。</returns>
		size_t GetCurrentIndex() const { return currentIndex_; }



	private:
		// コンボ移行ボタン
		std::vector<ComboButton> comboButtons_;
		size_t currentIndex_ = 0;	// 次に成立させる入力の位置
		bool sequential_ = false;	// trueなら登録順に1つずつ評価する
	};



	

}

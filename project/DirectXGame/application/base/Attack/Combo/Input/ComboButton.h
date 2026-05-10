#pragma once
#include <DirectXGame/engine/input/Input.h>

#include "vector"


class InputSystem;
namespace Character {
	struct CharacterContext;
	class BaseCharacter;
}

namespace Combo {

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

	GamePadButton ConvertGamePadButton(ComboGamePadButton button);

	/// <summary>
	/// コンボボタン1つ分
	/// </summary>
	class ComboButton {
	public:
		// コンストラクタ
		ComboButton(ComboGamePadButton button, ComboButtonInputType type) : button_(button), type_(type) {}

		// 押したら
		bool IsPressed(const InputSystem& inputSystem) const;

		// 押した瞬間
		bool IsTriggered(const InputSystem& inputSystem) const;

		// 離した瞬間
		bool IsReleased(const InputSystem& inputSystem) const;


			// 押して反応する条件
			bool IsInput(const InputSystem& inputSystem) const;

		// どのボタンに反応するか
		void SetGamePadButton(ComboGamePadButton button) { button_ = button; };

	private:
		ComboGamePadButton button_;
		ComboButtonInputType type_ = ComboButtonInputType::kPressed;
	};


	/// <summary>
	/// コンボ（ボタンの順番を管理）
	/// </summary>
	class ComboSequence {
	public:
		/// <summary>
		/// コンボボタンを順番に登録
		/// </summary>
		void RegisterCombo(const std::vector<ComboButton>& buttons);

		/// <summary>
		/// コンボ成立チェック
		/// </summary>
		bool Update(const Character::CharacterContext& ctx);



	private:
		// コンボ移行ボタン
		std::vector<ComboButton> comboButtons_;
		size_t currentIndex_ = 0;
	};



	

}
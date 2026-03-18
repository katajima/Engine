#pragma once
#include "DirectXGame/engine/struct/Vector3.h"
#include "DirectXGame/application/base/Attack/AttackData.h"
#include "DirectXGame/application/base/Character/Base/CharacterContext.h"

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

	/// <summary>
	/// コンボボタン1つ分
	/// </summary>
	class ComboButton {
	public:
		// コンストラクタ
		ComboButton(GamePadButton button, ComboButtonInputType type) : button_(button), type_(type) {}

		// 押したら
		bool IsPressed(const InputSystem& inputSystem) const {
			return inputSystem.GetButtom(InputButton::kPressed,button_);
		}

		// 押した瞬間
		bool IsTriggered(const InputSystem& inputSystem) const {
			return inputSystem.GetButtom(InputButton::kTriggered, button_);
		}

		// 離した瞬間
		bool IsReleased(const InputSystem& inputSystem) const {
			return inputSystem.GetButtom(InputButton::kReleased, button_);
		}


		// 押して反応する条件
		bool IsInput(const InputSystem& inputSystem) const {

			switch (type_)
			{
			case ComboButtonInputType::kPressed: // 押したら
				return IsPressed(inputSystem);
				break;
			case ComboButtonInputType::kTriggered: // 押した瞬間
				return IsTriggered(inputSystem);
				break;
			case ComboButtonInputType::kReleased: // 離した瞬間
				return IsReleased(inputSystem);
				break;
			case ComboButtonInputType::kPressTriggerReleased: // 押す、押した瞬間、離した瞬間
				return IsPressed(inputSystem) || IsTriggered(inputSystem) || IsReleased(inputSystem);
				break;
			case ComboButtonInputType::kPressTriggered:
				return IsPressed(inputSystem) || IsTriggered(inputSystem); // 押す、押した瞬間
				break;
			case ComboButtonInputType::kPressReleased:
				return IsPressed(inputSystem) || IsReleased(inputSystem); // 押す、離した瞬間
				break;
			case ComboButtonInputType::kTriggerReleased:
				return IsTriggered(inputSystem) || IsReleased(inputSystem); // 押した瞬間、離した瞬間
				break;
			default:	// 指定されたtypeでないのなら
				return false;
				break;
			}
		}

		// どのボタンに反応するか
		void SetGamePadButton(GamePadButton button) { button_ = button; };

	private:
		GamePadButton button_;
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
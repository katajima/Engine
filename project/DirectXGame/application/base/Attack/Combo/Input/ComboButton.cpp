#include "ComboButton.h"

#pragma region ComboSequence

/// <summary>
/// コンボボタンを順番に登録
/// </summary>
void Combo::ComboSequence::RegisterCombo(const std::vector<ComboButton>& buttons) {
	comboButtons_.clear();
	for (auto& b : buttons) {
		comboButtons_.emplace_back(b);
	}
}

/// <summary>
/// コンボ成立チェック
/// </summary>
bool Combo::ComboSequence::Update(const Character::CharacterContext& ctx) {
	if (comboButtons_.empty()) return false;


	// 次に押すべきボタン
	const ComboButton& target = comboButtons_[currentIndex_];

	// 入力判定
	if (target.IsInput(*ctx.input)) {
		currentIndex_++;
		// 全て成功
		if (currentIndex_ >= comboButtons_.size()) {
			currentIndex_ = 0;
			return true;
		}
	}
	currentIndex_ = 0;
	return false;
}

#pragma endregion // コンボボタン


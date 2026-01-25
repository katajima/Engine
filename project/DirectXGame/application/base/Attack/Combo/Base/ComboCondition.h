#pragma once
#include "DirectXGame/engine/struct/Vector3.h"
#include "DirectXGame/application/base/Attack/AttackData.h"


class BaseCharacter;		// キャラクター
class JumpSystem;			// ジャンプシステム


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
	bool IsPressed(const Engine::Input& input) const {
		return input.IsGamePadPressed(button_);
	}

	// 押した瞬間
	bool IsTriggered(const Engine::Input& input) const {
		return input.IsGamePadTriggered(button_);
	}

	// 離した瞬間
	bool IsReleased(const Engine::Input& input) const {
		return input.IsGamePadReleased(button_);
	}


	// 押して反応する条件
	bool IsInput(const Engine::Input& input) const {

		switch (type_)
		{
		case ComboButtonInputType::kPressed: // 押したら
			return IsPressed(input);
			break;
		case ComboButtonInputType::kTriggered: // 押した瞬間
			return IsTriggered(input);
			break;
		case ComboButtonInputType::kReleased: // 離した瞬間
			return IsReleased(input);
			break;
		case ComboButtonInputType::kPressTriggerReleased: // 押す、押した瞬間、離した瞬間
			return IsPressed(input) || IsTriggered(input) || IsReleased(input);
			break;
		case ComboButtonInputType::kPressTriggered:
			return IsPressed(input) || IsTriggered(input); // 押す、押した瞬間
			break;
		case ComboButtonInputType::kPressReleased:
			return IsPressed(input) || IsReleased(input); // 押す、離した瞬間
			break;
		case ComboButtonInputType::kTriggerReleased:
			return IsTriggered(input) || IsReleased(input); // 押した瞬間、離した瞬間
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
	bool Update(const Engine::Input& input, float deltaTime);



private:
	// コンボ移行ボタン
	std::vector<ComboButton> comboButtons_;
	size_t currentIndex_ = 0;
};


// コンボ受付条件クラス
class ComboCondition {
public:

	// 終了条件タイプ
	enum class EndConditionType {
		kOnGround,			// 着地したら
		kOnButtonRelease,	// ボタンを離したら
		kOnMeterEmpty,		// メータが空になったら
		kOnTimer,			// 時間が過ぎたら
		kOnHit,				// 何かに当たったら
		kManual,			// 特殊ケース
	};


	// データ構造体
	struct InputData
	{
		float inputWindowStart_ = 0.1f;			// 入力受付スタート
		float inputWindowEnd_ = 0.5f;			// 入力受付エンド

		float cancelStart_ = 0.1f;				// キャンセル開始時間
		float cancelEnd_ = 0.5f;				// キャンセル終了時間


		bool isCancel_ = false;					// キャンセル可能かどうか


		bool isCompulsionNextCombo_ = false;	// 強制的に次のコンボに移行するか 
		ComboSequence comboSequence_;			// コンボボタン条件
	};

	// 終了条件データ
	struct EndData {
		float stateEndTime = 0.5f;				// 終了時間
		float stateNextTime = 0.45f;			// 移行時間
		ComboButton button_;					// コンボボタン
		EndConditionType type = EndConditionType::kOnTimer;	// 終了条件タイプ
	};


	// 開始
	void Enter(BaseCharacter* owner);

	// 更新
	void Update(const Engine::Input& input, float timer, float dt);

	// 終了
	void Exit();

private:
	// 終了条件設定
	void EndComboUpdate(const Engine::Input& input, float timer, float dt);
public: //設定
	// コンボ条件発動時間設定
	void ConditionStartEnd(float start, float end) {
		inputData_.inputWindowStart_ = start;
		inputData_.inputWindowEnd_ = end;
	};
	// コンボキャンセル時間設定
	void SetCancelTime(float start, float end) {
		inputData_.cancelStart_ = start;
		inputData_.cancelEnd_ = end;
	}
	// コンボ移行時間設定
	void SetComboNextTime(float time) { nextTime_ = time; }
	// コンボ終了時間設定
	void SetComboEndTime(float time) { endTime_ = time; }
public: // 取得

	// 次のコンボ移行する時間
	float GetComboNextTime() const { return nextTime_; }
	// コンボ終了時間
	float GetComboEndTime() const { return endTime_; }
	//

	// コンボ入力受付開始時間
	float GetComboInputStart() const { return inputData_.inputWindowStart_; }
	// コンボ入力受付終了時間
	float GetComboInputEnd() const { return inputData_.inputWindowEnd_; }
	// コンボ受付可能か
	bool IsComdoInputWindow(float timer) const {
		return timer >= GetComboInputStart() && timer <= GetComboInputEnd();
	};
	// 次のコンボに移行するか
	bool IsNextCombo() const { return isNextCombo_; };
	// コンボキャンセルするか
	bool IsComboCansel() const { return isCansel_; }
public:
	// データ構造体取得
	EndData& GetData() { return data_; }
	// 入力受付データ構造体取得
	InputData& GetInput() { return inputData_; }
private:
	bool isNextCombo_ = false;			// 次のコンボに移行フラグ
	bool isCansel_ = false;				// キャンセルするかのフラグ

	float endTime_ = 0.0f;				// コンボ終了時間
	float nextTime_ = 0.0f;				// 次のコンボ移行時間
	

	
	EndData data_{ 0.5f ,0.45f ,ComboButton(GamePadButton::GAMEPAD_B,ComboButtonInputType::kPressed) };// コンボ終了データ
	InputData inputData_;				// コンボ入力受付データ
	bool isPress_ = false;
private:
	JumpSystem* jumpSystem = nullptr;
};

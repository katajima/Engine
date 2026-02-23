#pragma once
#include "ComboConditionFunction.h"


namespace Combo {

	class NextCondition {
	public:
		// 移行条件データ
		struct Data {
			float stateTime = 0.45f;				// 移行時間
			EndConditionType type = EndConditionType::kOnTimer;	// 終了条件タイプ
			ComboButton button_ = ComboButton(GamePadButton::GAMEPAD_B, ComboButtonInputType::kPressed);			// コンボボタン
		};

		// 開始
		void Enter();
		//　終了
		void Exit();
		// 更新
		void Update(const InputSystem& inputSystem, MovementComponent* movementComponent, float time, float dt);
	public:
		Data& GetData() { return data_; }
		// 次のコンボ移行する時間
		float GetComboNextTime() const { return nextTime_; }
	private:
		Data data_;

		float nextTime_ = 0.0f;				// コンボ終了時間
		bool isPress_ = false;
	};


}

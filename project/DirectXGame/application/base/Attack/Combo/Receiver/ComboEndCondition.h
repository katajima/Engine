#pragma once
#include "ComboConditionFunction.h"

namespace Combo {

	class EndCondition {
	public:
		// 終了条件データ
		struct Data {
			float stateTime = 0.5f;				// 終了時間
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
		//
		Data& GetData() { return data_; }
		//
		float GetComboEndTime() const { return endTime_; }
	private:
		Data data_;

		float endTime_ = 0.0f;				// コンボ終了時間
		bool isPress_ = false;
	};

}
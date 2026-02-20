#pragma once
#include "DirectXGame/application/base/Attack/Combo/Base/ComboData.h"
#include "ComboState.h"

namespace Character {
	class BaseCharacter; // 前方宣言
}

namespace Engine {
	class GlobalVariables;
	class LineCommon;
}

namespace Combo {

	/// <summary>
	/// コンボ用デバッグ
	/// </summary>
	class ComboDebug {
	public:
		// 初期化
		void Initialize(Engine::LineCommon* lineCommon,Character::BaseCharacter* owner);

		// 更新
		void Update(float dt);

	private:
		Character::BaseCharacter* owner = nullptr;
		Engine::LineCommon* lineCommon = nullptr;
	};
}
#pragma once
#include "DirectXGame/application/base/Attack/Combo/Base/ComboData.h"
#include "DirectXGame/application/base/Attack/Combo/Base/ComboState.h"

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
		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize(Engine::LineCommon* lineCommon,Character::BaseCharacter* owner);

		/// <summary>
		/// 更新
		/// </summary>
		void Update(float dt);

	private:
		Character::BaseCharacter* owner = nullptr;
		Engine::LineCommon* lineCommon = nullptr;
	};
}
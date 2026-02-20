#pragma once
#include "CharacterData.h"
#include "DirectXGame/application/base/Move/Dash/DashData.h"
#include "DirectXGame/application/base/Move/Jump/JumpData.h"
#include "DirectXGame/application/base/Move/Move/MoveData.h"


namespace Character {

	/// <summary>
	/// キャラクター用保存項目データ
	/// </summary>
	struct GlobalData {

		// 名前
		std::string name = "character";


		/// 基本パラメータ ///

		// 体力
		float hp = 100.0f;
		// mp
		float mp = 100.0f;
		// スタミナ
		float stamina = 100.0f;
		
		/// 当たり判定 ///
		float colliderRadius = 3.0f;

		// 移動データ
		MoveData moveData;
		// ジャンプデータ
		JumpData jumpData;
		// ダッシュデータ
		DashData dashData;
	};






}
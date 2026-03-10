#pragma once
#include"DirectXGame/engine/3d/Object/Object3d.h"
#include "DashData.h"
#include "DirectXGame/application/base/Move/Base/LocomotionCoordinator.h"

/// <summary>
/// ダッシュに関するシステムクラス
/// </summary>
class DashSystem {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();
	// 更新
	void Update(const Character::CharacterContext& ctx, LocomotionCoordinator& coordinator);
public:
	// ダッシュ用データの取得
	DashData& GetData() { return data_; };
private:
	// ダッシュ用データ
	DashData data_{};
private:
	// タイマー
	float timer_ = 0.0f;

};


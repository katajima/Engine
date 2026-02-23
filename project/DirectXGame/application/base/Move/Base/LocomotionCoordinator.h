#pragma once
#include "LocomotionContext.h"
#include "DirectXGame/application/base/Input/InputSystem.h"
/// <summary>
/// 移動関連の調停役クラス
/// </summary>
class LocomotionCoordinator {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="inputSystem"></param>
	void BeginFrame(const LocomotionContext& ctx) {
		ctx_ = &ctx;
		built_ = false;
	}
	/// <summary>
	/// 移動コマンド生成
	/// </summary>
	/// <returns></returns>
	MoveCommand BuildCommand();
private:
	const LocomotionContext* ctx_ = nullptr;
	bool built_ = false;
};
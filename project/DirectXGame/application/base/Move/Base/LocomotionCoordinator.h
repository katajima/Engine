#pragma once
#include <vector>
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
	void BeginFrame(const Character::CharacterContext& ctx) {
		this->ctx = &ctx;
		built_ = false;
		requests_.clear();
	}

	/// <summary>
	/// リクエスト
	/// </summary>
	/// <param name="request"></param>
	void Request(MoveRequest request) { requests_.push_back(request); }

	/// <summary>
	/// 移動コマンド生成
	/// </summary>
	/// <returns></returns>
	MoveCommand BuildCommand();
private:
	const Character::CharacterContext* ctx = nullptr;
	bool built_ = false;
	std::vector<MoveRequest> requests_;
};
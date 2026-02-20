#pragma once
#include "DirectXGame/engine/input/Input.h"
#include "InputData.h"


/// <summary>
/// 入力の値の管理
/// </summary>
class InputSystem {
public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Engine::Input* input);


	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="dt"></param>
	void Update(float dt);
	
	// 入力データ取得
	InputData GetData() const { return data_; }

private:
	InputData data_;

private:
	// 入力クラス
	Engine::Input* input = nullptr;
};
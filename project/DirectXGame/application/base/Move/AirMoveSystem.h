#pragma once
#include"DirectXGame/engine/3d/Object/Object3d.h"


/// <summary>
/// 空中移動システム
/// </summary>
class AirMoveSystem {
public:

	struct Data {

	};

	// 初期化
	void Initialize();

	// 更新
	void Update(float dt);


public:
	// データ取得
	Data& GetData() { return data_; }

private:
	// データ
	Data data_;

};


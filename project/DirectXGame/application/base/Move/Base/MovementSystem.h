#pragma once


/// <summary>
/// 移動リクエストを元に最終的な移動速度→トランスフォーム更新をこなうクラス
/// </summary>
class MovementSystem {
public:

	/// <summary>
	/// 更新
	/// </summary>
	void Initialize();


	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="dt"></param>
	void Update(float dt);

private:

};
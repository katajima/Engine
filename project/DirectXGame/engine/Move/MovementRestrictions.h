#pragma once
#include <DirectXGame/engine/Transform/WorldTransform/WorldTransform.h>

// 移動制限クラス
/// <summary>
/// MovementRestrictionsを管理・実装するクラス。
/// </summary>
class MovementRestrictions {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Vector3 min,Vector3 max);
	/// <summary>
	/// 更新
	/// </summary>
	void Update(Engine::WorldTransform& world);

private:
	Vector3 min_ = { 0,0,0 }; // 最小座標 
	Vector3 max_ = { 0,0,0 }; // 最大座標
};




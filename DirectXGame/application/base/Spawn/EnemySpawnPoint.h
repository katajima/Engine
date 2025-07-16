#pragma once
#include "DirectXGame/application/base/BaseClass/Spawn/BaseSpawnPoint.h"



class EnemySpawnPoint : public BaseSpawnPoint
{
public:
	///< summary>
	/// 初期化
	///</summary>
	void Initialize(Input* input, Entity3DManager* entity3DManager, Entity2DManager* entity2DManager,
		GlobalVariables* globalVariables, Vector3 position, Camera* camera) override;

	///< summary>
	/// 更新
	///</summary>
	void Update()override;

	/// <summary>
	/// エフェクトの描画
	/// </summary>
	void DrawEffect() override;

	/// <summary>
	/// 2d描画
	/// </summary>
	void Draw2D() override;

protected:



};



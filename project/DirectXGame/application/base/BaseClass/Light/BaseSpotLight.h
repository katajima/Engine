#pragma once
#include "BaseLight.h"

class BaseSpotLight : public BaseLight
{
public:
	///< summary>
	/// 初期化
	///</summary>
	virtual void Initialize(Input* input, Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, GlobalVariables* globalVariables, Vector3 position, Camera* camera) = 0;

	///< summary>
	/// 更新
	///</summary>
	virtual void Update() = 0;

	/// <summary>
	/// エフェクトの描画
	/// </summary>
	virtual void DrawEffect() = 0;


	virtual void Draw2D() = 0;

protected:
	// ライト
	std::shared_ptr<SpotLight> spotLight_;
	// スポットライトデータ
	SpotLightData spotLightData{};
};

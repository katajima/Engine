#pragma once
#include "BaseLight.h"

/// <summary>
/// ディレクショナルライト
/// </summary>
class BaseDirectionalLight : public BaseLight
{
public:
	///< summary>
	/// 初期化
	///</summary>
	virtual void Initialize(Input* input, Entity3DManager* entity3DManager, GlobalVariables* globalVariables, Vector3 position) = 0;

	///< summary>
	/// 更新
	///</summary>
	virtual void Update() = 0;

	/// <summary>
	/// エフェクトの描画
	/// </summary>
	virtual void DrawEffect() = 0;

	/// <summary>
	/// 2d描画
	/// </summary>
	virtual void Draw2D() = 0;

protected:
	// ライト
	std::shared_ptr<DirectionalLight> directionalLight_;
	// 平行ライトデータ
	DirectionalLightData directionalLightData{};
};


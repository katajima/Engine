#pragma once
#include "BaseLight.h"

/// <summary>
/// スポットライト
/// </summary>
class BaseSpotLight : public BaseLight
{
public:
	///< summary>
	/// 初期化
	///</summary>
	virtual void Initialize(Engine::Input* input, Engine::EntityManager* entity3DManager, Engine::GlobalVariables* globalVariables, Vector3 position) = 0;

	///< summary>
	/// 更新
	///</summary>
	virtual void Update() = 0;
protected:
	// ライト
	std::shared_ptr<Engine::SpotLight> spotLight_;
	// スポットライトデータ
	SpotLightData spotLightData{};
};

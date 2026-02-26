#pragma once
#include "DirectXGame/application/base/Object/ObjectComponent.h"

class Effect;
/// <summary>
/// プレイヤー車
/// </summary>
class PlayerCar
{
public:
	
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="entity3DManager"></param>
	/// <param name="globalVariables"></param>
	void Initialize(Engine::Entity3DManager* entity3DManager, Engine::GlobalVariables* globalVariables,const Vector3& pos, const Vector3& rotate);


	void SetGroungHeight(float height) { height_ = height; }

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="dt"></param>
	void Update(float dt);

	/// <summary>
	/// エフェクト
	/// </summary>
	/// <param name="effect"></param>
	void SetEffect(Effect* effect) { this->effect = effect; }


	/// <summary>
	/// 車位置
	/// </summary>
	/// <returns></returns>
	Vector3 GetPosition() { return objectComponent_->GetWorldPosition(); }
	
	// 動く
	void Action() { isMoving_ = true; }

private:
	std::unique_ptr<ObjectComponent> objectComponent_ = nullptr;

	// 影用オブジェクトコンポーネント
	std::unique_ptr<ObjectComponent> objectComponentShadow_ = nullptr;

	// タイヤ用オブジェクトコンポーネント
	std::unique_ptr<ObjectComponent> objectComponentTire01_ = nullptr;
	std::unique_ptr<ObjectComponent> objectComponentTire02_ = nullptr;
	std::unique_ptr<ObjectComponent> objectComponentTire03_ = nullptr;
	std::unique_ptr<ObjectComponent> objectComponentTire04_ = nullptr;


	// スポットライト用ワールド変換
	Engine::WorldTransform spotLightTransform01_;
	Engine::WorldTransform spotLightTransform02_;

	// スポットライト
	std::shared_ptr<Engine::SpotLight> spotLight01_ = nullptr;
	std::shared_ptr<Engine::SpotLight> spotLight02_ = nullptr;


	// 位置
	Vector3 pos_ = { -1.25f,0.05f,12.0f };

	// 速度
	Vector3 velocity_ = { 4.0f,0.0f,0.0f };

	// 動いているか
	bool isMoving_ = false;
	// 動いている時間
	float moveTimer_ = 0.0f;
	// 地面の高さ
	float height_ = 0.0f;
private:
	Engine::Entity3DManager* entity3DManager = nullptr;
	Engine::GlobalVariables* globalVariables = nullptr;
	// エフェクト
	Effect* effect = nullptr;
};

#pragma once
#include "DirectXGame/application/base/Object/ObjectComponent.h"
#include "PlayerCarStateMachine.h"
#include <DirectXGame/engine/Move/RigidBodyComponent.h>
#include <DirectXGame/engine/Light/Lights.h>

class EffectSystem;
namespace Engine {
	class EntityManager;
	class GlobalVariables;
	class AudioManager;
}
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
	void Initialize(Engine::EntityManager* entityManager,Engine::AudioManager* audioManager, Engine::GlobalVariables* globalVariables,const Vector3& pos, const Vector3& rotate);


	void SetGroundHeight(float height) { height_ = height; }

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="dt"></param>
	void Update(float dt);

	/// <summary>
	/// エフェクト
	/// </summary>
	/// <param name="effect"></param>
	void SetEffect(EffectSystem* effect) { this->effect = effect; }

	/// <summary>
	/// 動く
	/// </summary>
	void Action() { isMoving_ = true; }

private:
	/// <summary>
	/// 影の更新
	/// </summary>
	void ShadowUpdate(float dt);
	/// <summary>
	/// タイヤの更新
	/// </summary>
	void TireUpdate(float dt);
	/// <summary>
	/// スポットライトの更新
	/// </summary>
	void SpotLightUpdate(float dt);
public:
	/// <summary>
	/// 車体ワールド変換を取得
	/// </summary>
	Engine::WorldTransform& GetBodyWorldTransform() { return objectComponent_->GetWorldTransform(); }
	/// <summary>
	/// 車体ワールド座標取得
	/// </summary>
	Vector3 GetBodyWorldPosition() const { return objectComponent_->GetWorldPosition(); }
	/// <summary>
	/// 左前タイヤのワールド変換を取得
	/// </summary>
	Engine::WorldTransform& GetLFTierWorldTransform() { return objectLFTier_->GetWorldTransform(); }
	/// <summary>
	/// 左後タイヤのワールド変換を取得
	/// </summary>
	Engine::WorldTransform& GetLBTierWorldTransform() { return objectLBTire_->GetWorldTransform(); }
	/// <summary>
	/// 右前タイヤのワールド変換を取得
	/// </summary>
	Engine::WorldTransform& GetRFTierWorldTransform() { return objectRFTire_->GetWorldTransform(); }
	/// <summary>
	/// 右後タイヤのワールド変換を取得
	/// </summary>
	Engine::WorldTransform& GetRBTierWorldTransform() { return objectRBTire_->GetWorldTransform(); }
	/// <summary>
	/// 左前タイヤのワールド座標取得
	/// </summary>
	Vector3 GetLFTierWorldPosition() const { return objectLFTier_->GetWorldTransform().GetWorldPosition(); }
	/// <summary>
	/// 左後タイヤのワールド座標取得
	/// </summary>
	Vector3 GetLBTierWorldPosition() const { return objectLBTire_->GetWorldTransform().GetWorldPosition(); }
	/// <summary>
	/// 右前タイヤのワールド座標取得
	/// </summary>
	Vector3 GetRFTierWorldPosition() const { return objectRFTire_->GetWorldTransform().GetWorldPosition(); }
	/// <summary>
	/// 右後タイヤのワールド座標取得
	/// </summary>
	Vector3 GetRBTierWorldPosition() const { return objectRBTire_->GetWorldTransform().GetWorldPosition(); }

	/// <summary>
	/// スクラップボックスのワールド変換を取得
	/// </summary>
	Engine::WorldTransform& GetScrapBoxWorldTransform() { return objectScrapBox_->GetWorldTransform(); }
	/// <summary>
	/// スクラップボックスのワールド座標取得
	/// </summary>
	Vector3 GetScrapBoxWorldPosition() const { return objectScrapBox_->GetWorldTransform().GetWorldPosition(); }

	/// <summary>
	/// 右前スポットライトのワールド変換を取得
	/// </summary>
	Engine::WorldTransform& GetRFSpotLightWorldTransform() { return spotLightTransformRF_; }
	/// <summary>
	/// 左前スポットライトのワールド変換を取得
	/// </summary>
	Engine::WorldTransform& GetLFSpotLightWorldTransform() { return spotLightTransformLF_; }

	/// <summary>
	/// 右前スポットライトを取得
	/// </summary>
	Engine::SpotLight* GetRFSpotLight() { return spotLightRF_.get(); }
	/// <summary>
	/// 左前スポットライトを取得
	/// </summary>
	Engine::SpotLight* GetLFSpotLight() { return spotLightLF_.get(); }

	

public:
	/// <summary>
	/// 埃を出す
	/// </summary>
	void Emit(const Vector3& pos);
	/// <summary>
	/// 埃を出す(方向付き)
	/// </summary>
	void Emit(const Vector3& pos, const Vector3& dir, const Vector3& range);

	void EmitScrapBox(const Vector3& pos, const Vector3& dir, const Vector3& range);

	/// <summary>
	/// ステートマシン取得
	/// </summary>
	PlayerCarStateMachine* GetStateMachine() { return stateMachine_.get(); }
private:
	// ステートマシン
	std::unique_ptr<PlayerCarStateMachine> stateMachine_ = nullptr;


	std::unique_ptr<ObjectComponent> objectComponent_ = nullptr;

	// 影用オブジェクトコンポーネント
	std::unique_ptr<ObjectComponent> objectComponentShadow_ = nullptr;

	// タイヤ用オブジェクトコンポーネント
	std::unique_ptr<ObjectComponent> objectLFTier_ = nullptr;
	std::unique_ptr<ObjectComponent> objectLBTire_ = nullptr;
	std::unique_ptr<ObjectComponent> objectRFTire_ = nullptr;
	std::unique_ptr<ObjectComponent> objectRBTire_ = nullptr;

	// スクラップボックス用オブジェクトコンポーネント
	std::unique_ptr<ObjectComponent> objectScrapBox_ = nullptr;


	// スポットライト用ワールド変換
	Engine::WorldTransform spotLightTransformRF_;
	Engine::WorldTransform spotLightTransformLF_;

	// スポットライト
	std::shared_ptr<Engine::SpotLight> spotLightRF_ = nullptr;
	std::shared_ptr<Engine::SpotLight> spotLightLF_ = nullptr;


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
	Engine::EntityManager* entityManager = nullptr;
	Engine::GlobalVariables* globalVariables = nullptr;
	Engine::AudioManager* audioManager = nullptr;
	// エフェクト
	EffectSystem* effect = nullptr;
};

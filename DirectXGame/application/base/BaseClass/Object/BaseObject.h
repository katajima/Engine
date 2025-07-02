#pragma once
#include"DirectXGame/engine/Camera/Camera.h"
#include"DirectXGame/engine/3d/Object/Object3d.h"
#include"DirectXGame/engine/2d/Sprite.h"
#include"DirectXGame/engine/base/Imgui/ImGuiManager.h"
#include"DirectXGame/engine/math/MathFanctions.h"
#include"DirectXGame/engine/input/Input.h"
#include"DirectXGame/engine/audio/Audio.h"
#include "DirectXGame/engine/Effect/Particle/ParticleEmitter.h"
#include "DirectXGame/engine/Effect/Particle/ParticleManager.h"


enum class ObjectType
{
	None = -1,	// なし
	Character,	// キャラクター
	Weapon,		// 武器
	Bullet,		// 弾
	Item,		// アイテム
	Gimmick,	// ギミック
	Environment,// 環境オブジェクト
	Effect,		// エフェクト
	Other,		// その他
};

class Entity3DManager;
class Entity2DManager;
class BaseObject : public IHitReceiver
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

	/// <summary>
	/// 2d描画
	/// </summary>
	virtual void Draw2D() = 0;


public:

	// オブジェクト3d取得
	Object3d* GetObject3D() { return objectBase_; }

	//
	WorldTransform& GetWorldTransform() { return objectBase_->worldtransform_; }

	// コライダーコンポーネント取得
	ColliderComponent* GetColliderComponent() { return objectBase_->GetColliderComponent(); }
	// コライダー衝突履歴削除
	void ColliderHistoryClear() { objectBase_->GetColliderComponent()->contactRecord_.Clear(); }
	// 衝突履歴取得
	ContactRecord& GetContactRecord() { return objectBase_->GetColliderComponent()->contactRecord_; }
	// オブジェクトタイプ取得
	ObjectType GetObjectType() const { return objectType_; }

protected:
	Object3d* objectBase_ = nullptr;// オブジェクト3d
	ObjectType objectType_ = ObjectType::None; // オブジェクトの種類
	std::string name_ = ""; // オブジェクト名
	float timeSpeed_ = 1.0f; // 時間の進む速さ(1.0fが通常、0.0fで停止、2.0fで2倍速など)
private:
	

protected: // 貰ってくるもの
	Entity3DManager* entity3DManager_ = nullptr;	// 3Dエンティティマネージャー
	Entity2DManager* entity2DManager_ = nullptr;	// 2Dエンティティマネージャー
	GlobalVariables* globalVariables_ = nullptr;	// グローバル変数
	Camera* camera_ = nullptr;						// カメラ
	Input* input_ = nullptr;						// 入力(使わないならnullptr)
	Audio* audio_ = nullptr;
};

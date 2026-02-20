#pragma once
#include"DirectXGame/engine/Camera/Camera.h"
#include"DirectXGame/engine/3d/Object/Object3d.h"
#include"DirectXGame/engine/3d/Object/Object3dInstansManager.h"
#include"DirectXGame/engine/2d/Sprite.h"
#include"DirectXGame/engine/base/Imgui/ImGuiManager.h"
#include"DirectXGame/engine/math/MathFunctions.h"
#include"DirectXGame/engine/audio/Audio.h"
#include<DirectXGame/engine/Effect/EffectComponent.h>
#include"DirectXGame/engine/Animation/AnimationData.h"
#include"DirectXGame/engine/collider/3d/ColliderComponent.h"

#include "DirectXGame/application/base/Input/InputSystem.h"

//オブジェクトタイプ
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
	Camera,		// カメラ
	Light,		// ライト
	Event,		// イベント
	Other,		// その他
};

// オブジェクトステートクラス
struct ObjectStateFlags
{
	bool isActive = true;		// アクティブ状態(デフォルトはtrue、falseで非アクティブ)
	bool isVisible = true;		// 可視状態(デフォルトはtrue、falseで非表示)
	bool isAlive = true;		// 生存状態(デフォルトはtrue、falseで死亡)
	bool isDeleted = false;		// 削除状態(デフォルトはfalse、trueで削除済み)
	bool isLockonTarget = false;// ロックオンターゲット状態(デフォルトはfalse、trueでロックオン対象)
};


// 前方宣言
namespace Engine {
	class Entity3DManager;
	class Entity2DManager;
	class ObjectInstans;
	class Camera;
}
/// <summary>
/// オブジェクトコンポーネントくクラス
/// </summary>
class ObjectComponent {
public:

	///< summary>
	/// 初期化
	///</summary>
	void Initialize(Engine::Entity3DManager* entity3DManager, Engine::GlobalVariables* globalVariables, 
		const std::string& objectName, const std::string& modelName, bool useCollider, bool useRigidBody, 
		IHitReceiver* iHitReceiver, Engine::ObjectModelType modelType = Engine::ObjectModelType::kNormal);

	/// <summary>
	/// インスタンシング用初期化
	/// </summary>
	void InitializeInstancing(Engine::Entity3DManager* entity3DManager, Engine::GlobalVariables* globalVariables,
		const std::string& objectName, const std::string& modelName, const std::string& texName, bool useCollider,
		bool useRigidBody, IHitReceiver* iHitReceiver,
		Engine::Object3dInstansManager::TransparencyType transparencyType);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();


public: // コライダー
	// コライダーコンポーネント取得
	Engine::ColliderComponent* GetColliderComponent();
	// コライダー衝突履歴削除
	void ColliderHistoryClear();
	// 衝突履歴取得
	Engine::ContactRecord& GetContactRecord();

	// 描画するか
	void SetIsDraw(bool is);

	// 削除
	void IsDelete();

public: // 取得
	// オブジェクト3d取得
	Engine::Object3d* GetObject3D() { return objectBase_; }
	// ワールド変換取得
	Engine::WorldTransform& GetWorldTransform();
	// ワールド座標取得
	Vector3 GetWorldPosition() { return GetWorldTransform().GetWorldPosition(); };
	// スクリーン座標取得
	Vector2 GetScreenPosition();
	// リジッドボディー取得
	Engine::RigidBodyComponent* GetRigidBodyComponent();

	// オブジェクト時間取得
	float GetTime() const;
	// 名前取得
	std::string GetName() const { return name_; }
	// 削除フラグ取得
	bool  GetDelete() const { return flags_.isDeleted; };
	// 削除する
	void Delete() { flags_.isDeleted = true; };
	// ロックオン状態の取得
	bool GetLockOn() const { return flags_.isLockonTarget; }
	// ロックオン状態を設定
	void SetLockOn(bool lock) { flags_.isLockonTarget = lock; }
	
public: // 設定

	// 名前設定
	void SetName(std::string name) { this->name_ = name; };

	// 時間設定
	void SetTimeSpeed(const float& spped);

	// オブジェクト状態取得(設定可能)
	ObjectStateFlags& GetObjectStateFlags() { return flags_; }
	// SRT設定
	void SetSRT(const Vector3& s = { 1,1,1 }, const Vector3& r = { 0,0,0 }, const Vector3& t = { 0,0,0 });
	// インスタンシング用SRT設定
	void SetInstancingSRT(const Vector3& s = { 1,1,1 }, const Vector3& r = { 0,0,0 }, const Vector3& t = { 0,0,0 });
	// 色の設定
	void SetColor(const Color& color);
private:
	Engine::Object3d* objectBase_ = nullptr;// オブジェクト3d(Entity3dManagerを使って)
	ObjectType objectType_ = ObjectType::None; // オブジェクトの種類(キャラクター、武器など)
	std::string name_ = "";		// オブジェクト名
	std::string modelName_ = "";		// オブジェクト名
	ObjectStateFlags flags_;	// オブジェクトの状態
	float timeSpeed_ = 1.0f;	// 時間の進む速さ(1.0fが通常、0.0fで停止、2.0fで2倍速など)
	bool useCollider_ = false;	// コライダーコンポーネントを使うか
	bool useInstancing = false;	// インスタンシング描画にするか


	Engine::ObjectInstans* objectInstance_ = nullptr;
	int instanceId_ = 0;

protected: // 貰ってくるもの
	Engine::Entity3DManager* entity3DManager_ = nullptr;	// 3Dエンティティマネージャー
	Engine::GlobalVariables* globalVariables_ = nullptr;	// グローバル変数
};

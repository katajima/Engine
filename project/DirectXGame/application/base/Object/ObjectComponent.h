#pragma once
#include"DirectXGame/engine/3d/Object/Object3d.h"
#include"DirectXGame/engine/3d/Object/ObjectInstance.h"
#include "DirectXGame/engine/3d/Model/RenderComponent.h"
#include"DirectXGame/engine/Animation/AnimationData.h"

#include <DirectXGame/application/GlobalVariables/GlobalVariables.h>

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
	class EntityManager;
	class Object3dInstanceManager;
	class Camera;
	class ColliderComponent;
	class RigidBodyComponent;
}
/// <summary>
/// オブジェクトコンポーネントくクラス
/// </summary>
class ObjectComponent {
public:

	/// <summary>通常描画オブジェクトと任意の物理コンポーネントを初期化する。</summary>
	/// <param name="entityManager">オブジェクトと各コンポーネントの生成元。</param>
	/// <param name="globalVariables">オブジェクト調整値の登録・保存先。不要な場合はnullptr。</param>
	/// <param name="objectName">EntityManager上の一意なオブジェクト名。</param>
	/// <param name="modelName">使用するモデルの登録名またはファイル名。</param>
	/// <param name="useCollider">コライダーを生成する場合はtrue。</param>
	/// <param name="useRigidBody">剛体を生成する場合はtrue。</param>
	/// <param name="iHitReceiver">衝突通知先。コライダー未使用時または通知不要時はnullptr。</param>
	/// <param name="modelType">通常、スキニングなどの描画モデル種別。</param>
	/// <param name="rigidUpdate">Update内で剛体を自動更新する場合はtrue。</param>
	void Initialize(Engine::EntityManager* entityManager, Engine::GlobalVariables* globalVariables, 
		const std::string& objectName, const std::string& modelName, bool useCollider, bool useRigidBody, 
		IHitReceiver* iHitReceiver, Engine::ObjectModelType modelType = Engine::ObjectModelType::kNormal,bool rigidUpdate = true);

	/// <summary>
	/// インスタンシング用初期化
	/// </summary>
	/// <param name="entityManager">インスタンスと各コンポーネントの生成元。</param>
	/// <param name="globalVariables">調整値の登録・保存先。不要な場合はnullptr。</param>
	/// <param name="objectName">インスタンス識別名。</param>
	/// <param name="modelName">使用するモデル名。</param>
	/// <param name="texName">上書きテクスチャ名。モデル既定を使う場合は空文字。</param>
	/// <param name="useCollider">コライダーを生成する場合はtrue。</param>
	/// <param name="useRigidBody">剛体を生成する場合はtrue。</param>
	/// <param name="iHitReceiver">衝突通知先。通知不要時はnullptr。</param>
	/// <param name="transparencyType">不透明または半透明の描画区分。</param>
	/// <param name="rigidUpdate">Update内で剛体を自動更新する場合はtrue。</param>
	void InitializeInstancing(Engine::EntityManager* entityManager, Engine::GlobalVariables* globalVariables,
		const std::string& objectName, const std::string& modelName, const std::string& texName, bool useCollider,
		bool useRigidBody, IHitReceiver* iHitReceiver,
		Engine::ObjectInstance::TransparencyType transparencyType, bool rigidUpdate = true);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();


public: // コライダー
	/// <summary>このオブジェクトへ接続したコライダーを取得する。</summary>
	/// <returns>EntityManagerが所有する非所有ポインター。未生成の場合はnullptr。</returns>
	Engine::ColliderComponent* GetColliderComponent();
	// コライダー衝突履歴削除
	void ColliderHistoryClear();
	/// <summary>コライダーが保持する接触履歴を取得する。</summary>
	/// <returns>次回履歴クリアまで有効な接触履歴への参照。</returns>
	Engine::ContactRecord& GetContactRecord();

	/// <summary>オブジェクトの描画可否を設定する。</summary>
	/// <param name="is">描画する場合はtrue。</param>
	void SetIsDraw(bool is);

	// 削除
	void IsDelete();

public: // 取得
	/// <summary>通常描画オブジェクトを取得する。</summary>
	/// <returns>EntityManagerが所有する非所有ポインター。インスタンシング利用時はnullptrの場合がある。</returns>
	Engine::Object3d* GetObject3D() { return objectBase_; }
	/// <summary>通常・インスタンシングを問わず、このオブジェクトのワールド変換を取得する。</summary>
	/// <returns>オブジェクトの生存期間中有効な変更可能参照。</returns>
	Engine::WorldTransform& GetWorldTransform();
	// ワールド座標取得
	Vector3 GetWorldPosition() { return GetWorldTransform().GetWorldPosition(); };
	/// <summary>現在のワールド座標を描画カメラのスクリーン座標へ変換する。</summary>
	/// <returns>ピクセル単位のスクリーン座標。</returns>
	Vector2 GetScreenPosition();
	/// <summary>接続した剛体コンポーネントを取得する。</summary>
	/// <returns>EntityManagerが所有する非所有ポインター。未生成の場合はnullptr。</returns>
	Engine::RigidBodyComponent* GetRigidBodyComponent();
	/// <summary>Update内でコライダー・剛体を自動更新するか設定する。</summary>
	/// <param name="is">ObjectComponent側で更新する場合はtrue。</param>
	void SetIsUpdateColliderComponent(bool is);
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

	/// <summary>このオブジェクト固有の時間倍率を設定する。</summary>
	/// <param name="speed">時間倍率。1.0で通常、0.0で停止。</param>
	void SetTimeSpeed(float speed);

	// オブジェクト状態取得(設定可能)
	ObjectStateFlags& GetObjectStateFlags() { return flags_; }
	/// <summary>通常オブジェクトのスケール・回転・移動をまとめて設定する。</summary>
	/// <param name="s">各軸のスケール。</param>
	/// <param name="r">ラジアン単位のXYZ回転。</param>
	/// <param name="t">ワールドまたは親基準の移動量。</param>
	void SetSRT(const Vector3& s = { 1,1,1 }, const Vector3& r = { 0,0,0 }, const Vector3& t = { 0,0,0 });
	/// <summary>インスタンシングオブジェクトのスケール・回転・移動をまとめて設定する。</summary>
	/// <param name="s">各軸のスケール。</param>
	/// <param name="r">ラジアン単位のXYZ回転。</param>
	/// <param name="t">ワールドまたは親基準の移動量。</param>
	void SetInstancingSRT(const Vector3& s = { 1,1,1 }, const Vector3& r = { 0,0,0 }, const Vector3& t = { 0,0,0 });
	/// <summary>描画マテリアルまたはインスタンスの色を設定する。</summary>
	/// <param name="color">RGBA色。各成分は通常0.0～1.0。</param>
	void SetColor(const Color& color);
private:
	Engine::Object3d* objectBase_ = nullptr;// オブジェクト3d(Entity3dManagerを使って)
	ObjectType objectType_ = ObjectType::None; // オブジェクトの種類(キャラクター、武器など)
	std::string name_ = "";		// オブジェクト名
	std::string modelName_ = "";		// オブジェクト名
	ObjectStateFlags flags_;	// オブジェクトの状態
	float timeSpeed_ = 1.0f;	// 時間の進む速さ(1.0fが通常、0.0fで停止、2.0fで2倍速など)
	bool useCollider_ = false;	// コライダーコンポーネントを使うか
	bool useRigidBody_ = false;	// リジッドボディーコンポーネントを使うか
	bool useInstancing = false;	// インスタンシング描画にするか
	bool rigidUpdate_ = true;

	Engine::ObjectInstance* objectInstance_ = nullptr;
	int instanceId_ = 0;

protected: // 貰ってくるもの
	Engine::EntityManager* entityManager = nullptr;	// 3Dエンティティマネージャー
	Engine::GlobalVariables* globalVariables = nullptr;	// グローバル変数
};

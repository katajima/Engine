#include "ObjectComponent.h"  
#include "DirectXGame/engine/MyGame/MyGame.h"
#include <DirectXGame/engine/3d/Object/Object3dInstansManager.h>

/// <summary>
/// オブジェクトの時間取得
/// </summary>
/// <returns></returns>
float ObjectComponent::GetTime() const { return Engine::MyGame::GameTime() * timeSpeed_; }

/// <summary>
/// オブジェクトの時間設定
/// </summary>
/// <param name="spped"></param>
void ObjectComponent::SetTimeSpeed(const float& spped) { timeSpeed_ = spped; }


/// <summary>
/// SRT設定
/// </summary>
/// <param name="s"></param>
/// <param name="r"></param>
/// <param name="t"></param>
void ObjectComponent::SetSRT(const Vector3& s, const Vector3& r, const Vector3& t)
{
    // オブジェクトが存在するなら
    if (objectBase_) {
        objectBase_->GetWorldTransform().scale_ = s;    // スケール
        objectBase_->GetWorldTransform().rotate_ = r;   // 回転
        objectBase_->GetWorldTransform().translate_ = t;// 位置
    }
}

void ObjectComponent::SetInstancingSRT(const Vector3& s, const Vector3& r, const Vector3& t)
{
    
    objectInstance_->transform.scale_ = s;    // スケール
    objectInstance_->transform.rotate_ = r;   // 回転
    objectInstance_->transform.translate_ = t;// 位置
}

// 色の設定
void ObjectComponent::SetColor(const Color& color) {
    if(useInstancing){
        objectInstance_->color = color.ToVector4();
    }
    else {
        objectBase_->GetMaterial(0)->GetMaterialInstance().color = color;
	}
};


// ワールド変換取得
Engine::WorldTransform& ObjectComponent::GetWorldTransform() {
    
    // インスタンシング描画なら
    if (useInstancing) {
        return objectInstance_->transform;
    }
    else {
        return objectBase_->GetWorldTransform();
    }
}

Engine::RigidBodyComponent* ObjectComponent::GetRigidBodyComponent() {
    // インスタンシング描画なら
    if (useInstancing) {
        return objectInstance_->GetRigidBodyComponent();
    }
    else {
        return objectBase_->GetRigidBodyComponent();
    }
};


void ObjectComponent::SetIsDraw(bool is) {
    // インスタンシング描画なら
    if (useInstancing) {
        objectInstance_->isDraw_ = is;
    }
    else {
        objectBase_->SetIsDraw(is);
    }
}

void ObjectComponent::IsDelete() {
    // インスタンシング描画なら
    if (useInstancing) {
        objectInstance_->IsDelete();
    }
    else {
        objectBase_->IsDelete();
    }
};


// コライダーコンポーネント取得
Engine::ColliderComponent* ObjectComponent::GetColliderComponent() {
    // インスタンシング描画なら
    if (useInstancing) {
        return objectInstance_->GetColliderComponent();
    }
    else {
        return objectBase_->GetColliderComponent();
    }
}
// コライダー衝突履歴削除
void ObjectComponent::ColliderHistoryClear() {
    // インスタンシング描画なら
    if (useInstancing) {
        objectInstance_->GetColliderComponent()->contactRecord_.Clear();
    }
    else {
        objectBase_->GetColliderComponent()->contactRecord_.Clear();
    }
}
// 衝突履歴取得
Engine::ContactRecord& ObjectComponent::GetContactRecord() {
    // インスタンシング描画なら
    if (useInstancing) {
        return objectInstance_->GetColliderComponent()->contactRecord_;
    }
    else {
        return objectBase_->GetColliderComponent()->contactRecord_;
    }
}

// スクリーン座標取得
Vector2 ObjectComponent::GetScreenPosition() {
    // インスタンシング描画なら
    if (useInstancing) {
        return ScreenPosition(objectInstance_->transform, entityManager->GetObject3dCommon()->GetDefaltCamera());
    }
    else {
        return objectBase_->GetScreenPosition();
    }
}

void ObjectComponent::SetIsUpdateColliderComponent(bool is) {
    // インスタンシング描画なら
    if (useInstancing) {
        return objectInstance_->SetIsUpdateColliderComponent(is);
    }
    else {
        return objectBase_->SetIsUpdateColliderComponent(is);
    }
};


/// <summary>
/// 初期化
/// </summary>
/// <param name="entity3DManager"></param>
/// <param name="globalVariables"></param>
void ObjectComponent::Initialize(Engine::EntityManager* entityManager, Engine::GlobalVariables* globalVariables, 
    const std::string& objectName, const std::string& modelName,bool useCollider, bool useRigidBody,
    IHitReceiver* iHitReceiver, Engine::ObjectModelType modelType,bool rigidUpdate) {

    this->entityManager = entityManager;   // エンティティ3d
    this->globalVariables = globalVariables;   // 保存項目
    timeSpeed_ = 1.0f;                          // タイムスピードを1.0fに設定
    useCollider_ = useCollider;                 // コライダーコンポーネントを使うか
    rigidUpdate_ = rigidUpdate;
    name_ = objectName;

    // オブジェクト生成
    objectBase_ = entityManager->CreateObject3D(name_, modelType, {}, nullptr);
    objectBase_->SetModel(modelName);                           // モデル指定
    SetSRT({1,1,1},{0,0,0},{0,0,0});
    // コライダ使うか
    if (useCollider) {
        objectBase_->InitColliderComponent();                   // コライダコンポーネント初期化
        GetColliderComponent()->SetHitReceiver(iHitReceiver);   // 対象設定
       
    }
    if (useRigidBody) {
        objectBase_->InitRigidBodyComponent();                  // 
        objectBase_->SetIsRigidUpdate(rigidUpdate); 
    }
}

void ObjectComponent::InitializeInstancing(Engine::EntityManager* entityManager, Engine::GlobalVariables* globalVariables,
    const std::string& objectName, const std::string& modelName, const std::string& texName, bool useCollider, bool useRigidBody,
    IHitReceiver* iHitReceiver, Engine::Object3dInstansManager::TransparencyType transparencyType, bool rigidUpdate)
{
    this->entityManager = entityManager;   // エンティティ3d
    this->globalVariables = globalVariables;   // 保存項目
    timeSpeed_ = 1.0f;                          // タイムスピードを1.0fに設定
    useCollider_ = useCollider;                 // コライダーコンポーネントを使うか
    useRigidBody_ = useRigidBody;
    rigidUpdate_ = rigidUpdate;
    name_ = objectName;                         // 名前
    modelName_ = modelName;                     // モデル名
    useInstancing = true;                       // インスタンシング描画にする
    // インスタンス初期化
    Engine::ObjectInstans object;
    object.Initialize(entityManager, useCollider_, rigidUpdate_);
    // オブジェクト追加
    entityManager->GetObject3dInstansManager()->AddObject(modelName, texName, std::move(object), instanceId_,
        Engine::Object3dInstansManager::MeshType::kModel,transparencyType);

    // インスタンス
    objectInstance_ = entityManager->GetObject3dInstansManager()->GetObjectById(modelName_, instanceId_, transparencyType);
}

/// <summary>
/// 更新
/// </summary>
void ObjectComponent::Update() {
    // インスタンシング描画なら
    if (useInstancing)
        objectInstance_->Update();
};
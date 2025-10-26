#include "ObjectComponent.h"  
#include "DirectXGame/engine/MyGame/MyGame.h"
#include <DirectXGame/engine/3d/Object/Object3dInstansManager.h>

/// <summary>
/// オブジェクトの時間取得
/// </summary>
/// <returns></returns>
float ObjectComponent::GetTime() const { return MyGame::GameTime() * timeSpeed_; }

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
    if (objectBase_) {
        objectBase_->GetWorldTransform().scale_ = s;
        objectBase_->GetWorldTransform().rotate_ = r;
        objectBase_->GetWorldTransform().translate_ = t;
    }
}

void ObjectComponent::SetInstancingSRT(const Vector3& s, const Vector3& r, const Vector3& t)
{
    
    objectInstance_->transform.scale_ = s;
    objectInstance_->transform.rotate_ = r;
    objectInstance_->transform.translate_ = t;
}




// ワールド変換取得
WorldTransform& ObjectComponent::GetWorldTransform() {
    if (useInstancing) {
        return objectInstance_->transform;
    }
    else {
        return objectBase_->GetWorldTransform();
    }
}

RigidBodyComponent* ObjectComponent::GetRigidBodyComponent() {
    if (useInstancing) {
        return objectInstance_->GetRigidBodyComponent();
    }
    else {
        return objectBase_->GetRigidBodyComponent();
    }
};


void ObjectComponent::SetIsDraw(bool is) {
    if (useInstancing) {
        objectInstance_->isDraw_ = is;
    }
    else {
        objectBase_->SetIsDraw(is);
    }
}

void ObjectComponent::IsDelete() {
    if (useInstancing) {
        objectInstance_->IsDelete();
    }
    else {
        objectBase_->IsDelete();
    }
};


// コライダーコンポーネント取得
ColliderComponent* ObjectComponent::GetColliderComponent() {
    if (useInstancing) {
        return objectInstance_->GetColliderComponent();
    }
    else {
        return objectBase_->GetColliderComponent();
    }
}
// コライダー衝突履歴削除
void ObjectComponent::ColliderHistoryClear() {
    if (useInstancing) {
        objectInstance_->GetColliderComponent()->contactRecord_.Clear();
    }
    else {
        objectBase_->GetColliderComponent()->contactRecord_.Clear();
    }
}
// 衝突履歴取得
ContactRecord& ObjectComponent::GetContactRecord() {
    if (useInstancing) {
        return objectInstance_->GetColliderComponent()->contactRecord_;
    }
    else {
        return objectBase_->GetColliderComponent()->contactRecord_;
    }
}


/// <summary>
/// 初期化
/// </summary>
/// <param name="entity3DManager"></param>
/// <param name="globalVariables"></param>
void ObjectComponent::Initialize(Entity3DManager* entity3DManager,  GlobalVariables* globalVariables, const std::string& objectName, const std::string& modelName,bool useCollider, bool useRigidBody,IHitReceiver* iHitReceiver, ObjectModelType modelType) {
    this->entity3DManager_ = entity3DManager;   // 
    this->globalVariables_ = globalVariables;   // 
    timeSpeed_ = 1.0f;                          // タイムスピードを1.0fに設定
    useCollider_ = useCollider;                 // コライダーコンポーネントを使うか
    name_ = objectName;


    objectBase_ = entity3DManager_->CreateObject3D(name_, modelType, {}, nullptr);
    objectBase_->SetModel(modelName);
    SetSRT({1,1,1},{0,0,0},{0,0,0});
    // コライダ使うか
    if (useCollider) {
        objectBase_->InitColliderComponent();                   // コライダコンポーネント初期化
        GetColliderComponent()->SetHitReceiver(iHitReceiver);   // 対象設定
        if(useRigidBody)
        objectBase_->InitRigidBodyComponent();                  // 
    }


}

void ObjectComponent::InitializeInstancing(Entity3DManager* entity3DManager, GlobalVariables* globalVariables, const std::string& objectName, const std::string& modelName, const std::string& texName, bool useCollider, bool useRigidBody, IHitReceiver* iHitReceiver)
{
    this->entity3DManager_ = entity3DManager;   // 
    this->globalVariables_ = globalVariables;   // 
    timeSpeed_ = 1.0f;                          // タイムスピードを1.0fに設定
    useCollider_ = useCollider;                 // コライダーコンポーネントを使うか
    name_ = objectName;
    modelName_ = modelName;
    useInstancing = true;
   

    ObjectInstans object;

    object.Initialize(entity3DManager_,true);

    entity3DManager_->GetObject3dInstansManager()->AddObject(modelName, texName, std::move(object), instanceId_);

    objectInstance_ = entity3DManager_->GetObject3dInstansManager()->GetObjectById(modelName_, instanceId_);
}

/// <summary>
/// 更新
/// </summary>
void ObjectComponent::Update() {
    if (useInstancing)
        objectInstance_->Update();
};
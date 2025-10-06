#include "ObjectComponent.h"  
#include "DirectXGame/engine/MyGame/MyGame.h"


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
    objectBase_->GetWorldTransform().scale_ = s;
    objectBase_->GetWorldTransform().rotate_ = r;
    objectBase_->GetWorldTransform().translate_ = t;
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

/// <summary>
/// 更新
/// </summary>
void ObjectComponent::Update() {

};
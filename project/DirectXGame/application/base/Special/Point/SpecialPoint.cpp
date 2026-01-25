#include "SpecialPoint.h"
#include "DirectXGame/engine/MyGame/MyGame.h"
#include "DirectXGame/engine/Manager/Effect/EffectManager.h"
#include "DirectXGame/engine/Manager/Entity3D/Entity3DManager.h"
#include "DirectXGame/engine/Manager/Entity2D/Entity2DManager.h"



#include <DirectXGame/application/base/Character/Base/Player/BasePlayer.h>

#pragma region Manager

void SpecalPointManager::Initialize(Engine::Entity3DManager* entity3DManager,
	Engine::Entity2DManager* entity2DManager, Engine::GlobalVariables* globalVariables) {

	entity3DManager_ = entity3DManager;
	entity2DManager_ = entity2DManager;
	globalVariables_ = globalVariables;

}

void SpecalPointManager::Update(float dt) {


	// 削除
	points.erase(
		std::remove_if(points.begin(), points.end(),
			[](const std::unique_ptr<SpecalPoint>& spPoint) {
				return !spPoint->GetAlive();
			}),
		points.end());

	// 更新
	for (auto& point : points) {
		point->SetTargetPos(target_->GetObjectComponent()->GetWorldPosition());
		point->Update(dt);
	}

}

void SpecalPointManager::AddPoint(const Vector3& pos, int point){
	std::unique_ptr<SpecalPoint> spPoint = std::make_unique<SpecalPoint>();
	spPoint->Initialize(entity3DManager_,globalVariables_,pos, point,count_);
	points.push_back(std::move(spPoint));
	count_++;
};

#pragma endregion // マネージャー



#pragma region Point

void SpecalPoint::Initialize(Engine::Entity3DManager* entity3DManager, Engine::GlobalVariables* globalVariables, const Vector3& pos, int point, int id) {
	id_ = id;			// ID
	point_ = point;		// ポイント量

	// オブジェクトコンポーネント追加
	objectComponent_ = std::make_unique<ObjectComponent>();
	// オブジェクトインスタンシング初期化
	objectComponent_->InitializeInstancing(entity3DManager, globalVariables, "" + std::to_string(id), "point.obj", "", 
		true, true, this, Engine::Object3dInstansManager::TransparencyType::kNo);
	objectComponent_->GetColliderComponent()->SetHitReceiver(this);	// インターフェース設定	
	objectComponent_->SetInstancingSRT({0.5f,0.5f,0.5f },{}, pos);

	objectComponent_->SetColor({ 1, 0.271f, 0 ,0});



	Engine::ColliderComponent* colliderComponent = objectComponent_->GetColliderComponent();
	// SphereColliderを追加
	auto sphere = std::make_unique<Engine::SphereCollider>();
	sphere->Enable();					// コライダ有効
	sphere->tag = CollisionTag::Item;	// タグ設定
	sphere->layer = CollisionLayer::Environment;// レイヤー設定
	sphere->radius = 2.0f; // 半径を適宜設定
	colliderComponent->AddCollider(std::move(sphere));	// コライダ追加

	// コールバック登録（例：プレイヤーと衝突したらダメージ）
	colliderComponent->onHitCallback = [this](Engine::Collider* self, Engine::Collider* other) {
		// プレイヤーかチェック
		auto* otherComponent = static_cast<Engine::ColliderComponent*>(other->owner);
		if (!otherComponent) return;
		if (other->tag != CollisionTag::Player) return;
			

			objectComponent_->GetWorldTransform().scale_ = { 0,0,0 };
			objectComponent_->Delete();
			Die();
			BasePlayer* player = static_cast<BasePlayer*>(otherComponent->GetHitReceiver());
			player->AddSpGauge(point_);
		};


	specialPointStateMachine_ = std::make_unique<SpecialPointStateMachine>();
	specialPointStateMachine_->RegisterState(SpecialPointState::kPop, [](SpecalPoint* p) {
		return std::make_unique<SpecialPointPopState>(p);
		});
	specialPointStateMachine_->RegisterState(SpecialPointState::kIdle, [](SpecalPoint* p) {
		return std::make_unique<SpecialPointIdleState>(p);
		});
	specialPointStateMachine_->RegisterState(SpecialPointState::kMove, [](SpecalPoint* p) {
		return std::make_unique<SpecialPointMoveState>(p);
		});
	specialPointStateMachine_->RegisterState(SpecialPointState::kEnd, [](SpecalPoint* p) {
		return std::make_unique<SpecialPointEndState>(p);
		});
	specialPointStateMachine_->Initialize(this);
}


void SpecalPoint::Update(float dt) {

	// ステートマシーン更新
	specialPointStateMachine_->Update(dt);
}

bool SpecalPoint::IsMove()
{
	return radius_ > Length(GetTargetPos() - GetObjectComponent()->GetWorldPosition());
}

#pragma endregion // 点



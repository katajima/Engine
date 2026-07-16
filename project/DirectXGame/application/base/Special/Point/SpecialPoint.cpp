#include "SpecialPoint.h"
#include "DirectXGame/engine/MyGame/MyGame.h"
#include "DirectXGame/engine/Manager/Effect/EffectManager.h"
#include "DirectXGame/engine/Manager/Entity/EntityManager.h"

#include <DirectXGame/application/base/Character/Player/Base/BasePlayer.h>

#pragma region Manager

void SpecialPointManager::Initialize(Engine::EntityManager* entityManager, Engine::GlobalVariables* globalVariables) {

	this->entityManager = entityManager;
	this->globalVariables = globalVariables;

}

void SpecialPointManager::Update(float dt) {


	// 削除
	points.erase(
		std::remove_if(points.begin(), points.end(),
			[](const std::unique_ptr<SpecialPoint>& spPoint) {
				return !spPoint->GetAlive();
			}),
		points.end());

	if (target)
	// 更新
	for (auto& point : points) {
		point->SetTargetPos(target->GetObjectComponent()->GetWorldPosition());
		point->Update(dt);
	}

}

void SpecialPointManager::AddPoint(const Vector3& pos, int point){
	std::unique_ptr<SpecialPoint> spPoint = std::make_unique<SpecialPoint>();
	spPoint->Initialize(entityManager,globalVariables,pos, point,count_);
	points.push_back(std::move(spPoint));
	count_++;
};

#pragma endregion // マネージャー



#pragma region Point

void SpecialPoint::Initialize(Engine::EntityManager* entityManager, Engine::GlobalVariables* globalVariables, const Vector3& pos, int point, int id) {
	id_ = id;			// ID
	point_ = point;		// ポイント量

	// オブジェクトコンポーネント追加
	objectComponent_ = std::make_unique<ObjectComponent>();
	// オブジェクトインスタンシング初期化
	objectComponent_->InitializeInstancing(entityManager, globalVariables, "" + std::to_string(id), "point.obj", "", 
		true, true, this, Engine::ObjectInstance::TransparencyType::kNo);
	objectComponent_->GetColliderComponent()->SetHitReceiver(this);	// インターフェース設定	
	objectComponent_->SetInstancingSRT({0.5f,0.5f,0.5f },{}, pos);

	objectComponent_->SetColor({ 1, 0.271f, 0 ,0});



	Engine::ColliderComponent* colliderComponent = objectComponent_->GetColliderComponent();
	// SphereColliderを追加
	auto sphere = std::make_unique<Engine::SphereCollider>();
	sphere->Enable();					// コライダ有効
	sphere->SetTag(CollisionTag::Item);	// タグ設定
	sphere->SetLayer(CollisionLayer::Environment);// レイヤー設定
	sphere->radius = 1.0f; // 半径を適宜設定
	colliderComponent->AddCollider(std::move(sphere));	// コライダ追加

	// コールバック登録（例：プレイヤーと衝突したらダメージ）
	colliderComponent->onHitCallback = [this](Engine::Collider* self, Engine::Collider* other) {
		// プレイヤーかチェック
		auto* otherComponent = static_cast<Engine::ColliderComponent*>(other->GetOwner());
		if (!otherComponent) return;
		if (other->GetTag() != CollisionTag::Player) return;
			

			objectComponent_->GetWorldTransform().scale_ = { 0,0,0 };
			objectComponent_->Delete();
			Die();
			Character::BasePlayer* player = static_cast<Character::BasePlayer*>(otherComponent->GetHitReceiver());
			player->AddSpGauge(point_);
		};


	specialPointStateMachine_ = std::make_unique<SpecialPointStateMachine>();
	specialPointStateMachine_->RegisterState(SpecialPointState::kPop, [](SpecialPoint* p) {
		return std::make_unique<SpecialPointPopState>(p);
		});
	specialPointStateMachine_->RegisterState(SpecialPointState::kIdle, [](SpecialPoint* p) {
		return std::make_unique<SpecialPointIdleState>(p);
		});
	specialPointStateMachine_->RegisterState(SpecialPointState::kMove, [](SpecialPoint* p) {
		return std::make_unique<SpecialPointMoveState>(p);
		});
	specialPointStateMachine_->RegisterState(SpecialPointState::kEnd, [](SpecialPoint* p) {
		return std::make_unique<SpecialPointEndState>(p);
		});
	specialPointStateMachine_->Initialize(this);
}


void SpecialPoint::Update(float dt) {

	// ステートマシーン更新
	specialPointStateMachine_->Update(dt);
}

bool SpecialPoint::IsMove()
{
	return radius_ > Length(GetTargetPos() - GetObjectComponent()->GetWorldPosition());
}

#pragma endregion // 点



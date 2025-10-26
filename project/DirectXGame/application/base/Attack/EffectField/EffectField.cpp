#include "EffectField.h"
#include "DirectXGame/engine/Manager/Entity3D/Entity3DManager.h"
#include "DirectXGame/engine/Manager/Entity2D/Entity2DManager.h"
#include "DirectXGame/engine/MyGame/MyGame.h"

void EffectField::Initialize(Entity3DManager* entity3DManager)
{
	entity3DManager_ = entity3DManager;



	// コライダーコンポーネントの初期化
	colliderComponent_ = std::make_unique<ColliderComponent>();
	colliderComponent_->SetOwner(colliderComponent_.get());
	// ラインコモンをセット
	colliderComponent_->SetLineCommon(entity3DManager_->Get3DLineCommon());
	// 登録（IDを取得したければ変数で受ける）
	colliderComponent_->SetUniqueId(UniqueIdGenerator::Generate());

	// 固有の初期化
	InitializeUniqe();
}


// 更新
void EffectField::Update() {

	time_ += MyGame::GameTime();

	// 存続時間を超えたら削除フラグを立てる
	if (deleteTime_ < time_) {
		deleteFlag_ = true;
	}

	// 固有の更新
	UpdateUniqe();
};

// 描画
void EffectField::Draw() {
};

// 影響を与える
void EffectField::Effect(BaseCharacter* character) {
	
	// ダメージモジュールが使用可能なら
	if (damage_) {
		damage_->Effect(character);
	}

	// ノックバックモジュールが使用可能なら
	if(knockback_) {
		knockback_->Effect(character);
	}

	// 状態変更モジュールは使用可能なら
	if(changeState_) {
		changeState_->Effect(character);
	}

};

// ダメージモジュール使用
void EffectField::UseDamageModule() { 
	damage_ = std::make_unique<DamageModule>(); 
}

// ノックバックモジュール使用
void EffectField::UseKnockbackModule(){
	knockback_ = std::make_unique<KnockbackModule>();
}

void EffectField::UseChangeStateModule(){
	changeState_ = std::make_unique<ChangeStateModule>();
}

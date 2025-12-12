#include "HitBoxFunction.h"
#include <DirectXGame/engine/MyGame/MyGame.h>
#include"DirectXGame/application/base/Character/Base/Enemy/BaseEnemy.h"
#include "DirectXGame/application/base/Character/Base/Player/BasePlayer.h"

bool HitBoxFunction::Begin(Engine::Collider* self, Engine::Collider* other){
	other_ = static_cast<Engine::ColliderComponent*>(other->owner);
	otherColl_ = other;
	if (!other_) return false;

	const uint32_t otherId = other_->GetUniqueId();	// ID取得
	const float nowTime = Engine::MyGame::NowTime();		// 現在時間
	if (GetContactRecord().CheckHistory(otherId)) {
		return false; // クールタイム中のため無視
	}
	// 履歴追加
	GetContactRecord().AddHistory(otherId, nowTime);

	return true;
}


void HitBoxFunction::Update() {

	if (type_ == HitBoxUseType::kPlayer) {
		UpdateTypePlayer();
	}
	else if (type_ == HitBoxUseType::kEnemy) {
		UpdateTypeEnemy();
	}
	else {
		UpdateTypeOther();
	}
}

void HitBoxFunction::UpdateTypePlayer(){

	if (otherColl_->tag != CollisionTag::Enemy) return;

	BaseEnemy* enemy = static_cast<BaseEnemy*>(other_->GetHitReceiver());
	if (!enemy) return;

	BasePlayer* player = static_cast<BasePlayer*>(character_);
	if (!player) return;


	// ノックバック方向
	data_.GetKnockbackData().SetNormal(player->GetMoveComponent()->GetDirection());

	// リアクションデータ
	enemy->GetResponseSystem()->GetHitMotionSystem()->SetReactionData(data_);	 
	
	enemy->Emit();	//	エフェクト出現
	enemy->GetCharacterStateMachine()->ChangeState(CharacterMainState::Move); // 敵ステート設定

	// ヒットカウンターにヒットを通知
	player->GetAttackController()->GetHitCounter().Hit();
	// スペシャルゲージ増化
	player->AddSpGauge(1);
}

void HitBoxFunction::UpdateTypeEnemy(){
}

void HitBoxFunction::UpdateTypeOther(){
}


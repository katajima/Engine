#include "DeathSystem.h"
#include <DirectXGame/application/base/Character/Base/BaseCharacter.h>
#include "DirectXGame/engine/Entity/ObjectComponent.h"

void DeathSystem::Update(float dt){
	if (!isActive) return;

	dieTimer += dt;



	if (dieTimer >= data.dieTimer) {
		// 死亡完了
		isActive = false;
		dieTimer = 0.0f;

		// 死亡判定に
		owner->SetAlive(false);
		if (!owner->GetAlive()) {
			owner->Delete();	// キャラクター削除
			owner->GetObjectComponent()->IsDelete();	// オブジェクトコンポーネント削除
			owner->GetObjectComponent()->GetObjectStateFlags().isAlive = false;
			owner->GetObjectComponent()->SetIsDraw(false);	// 描画しない	
		}
	}

}

void DeathSystem::StartDeath(DeathType deathType,const DeathData& deathData){
	// タイプ設定
	type = deathType;
	// データ保存
	data = deathData;
	// 開始フラグ
	isActive = true;
	// タイマーリセット
	dieTimer = 0.0f;
}

void DeathSystem::TypeProcess(){
	




}

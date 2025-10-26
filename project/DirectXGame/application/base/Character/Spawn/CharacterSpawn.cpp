#include "CharacterSpawn.h"
#include"DirectXGame/application/base/Character/Base/BaseCharacterManeger.h"
#include"DirectXGame/engine/Line/LineCommon.h"
#include "DirectXGame/engine/MyGame/MyGame.h"

void CharacterSpawn::Initialize(BaseCharacterManager* characterManager, LineCommon* line, const SpawnInfo& info){
	characterManager_ = characterManager;
	lineCommon_ = line;

	spawnInfo_ = info;

	// トランスフォーム初期化
	spawnTransform_.Initialize();
}

void CharacterSpawn::Update(float time) {

	// トランスフォーム更新
	spawnTransform_.Update();

	spawnAABBArea_.min_ = spawnTransform_.GetWorldPosition() - (spawnInfo_.size_ / 2.0f);
	spawnAABBArea_.max_ = spawnTransform_.GetWorldPosition() + (spawnInfo_.size_ / 2.0f);

#ifdef _DEBUG
	lineCommon_->GetDebugLineMeshData().AddLineAABB(AABB{-spawnInfo_.size_,spawnInfo_.size_}, spawnTransform_.GetWorldPosition());
#endif // _DEBUG

	timer_ = time;

	if (timer_ >= spawnInfo_.spawnTimer_) {
		spawnInfo_.SetIsSpawn(true);
	}

	if (spawnInfo_.IsSpawn()) {
		SpawnProcess();
	}

}

void CharacterSpawn::Draw() {

}

void CharacterSpawn::SpawnProcess(){
	if (spawnInfo_.IsEnd()) return;


	spawnInfo_.Update(MyGame::GameTime());

	if (spawnInfo_.IsSpawned()) return;

	for (int i = 0; i < spawnInfo_.spawnAmount_; i++) {
		Vector3 pos = Random::RandomVector3(spawnAABBArea_.min_, spawnAABBArea_.max_);
		pos.y = 0.0f;

		characterManager_->CreateCharacter(EnemyType::kNormal, "", Transform({ 1,1,1 }, {}, pos));
	}


	spawnInfo_.Spawned();
}

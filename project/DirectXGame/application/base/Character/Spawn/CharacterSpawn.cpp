#include "CharacterSpawn.h"
#include"DirectXGame/application/base/Character/Base/CharacterManeger.h"
#include"DirectXGame/engine/Line/LineCommon.h"
#include "DirectXGame/engine/MyGame/MyGame.h"

void CharacterSpawn::Initialize(CharacterManager* characterManager, Engine::LineCommon* line, const SpawnInfo& info){
	characterManager_ = characterManager;	// キャラクター管理クラス
	lineCommon_ = line;						// ライン管理クラス
		
	spawnInfo_ = info;	// スポーン情報

	// トランスフォーム初期化
	spawnTransform_.Initialize();
	spawnTransform_.translate_ = spawnInfo_.GetData().translate_;
}

void CharacterSpawn::Update(float dt) {

	// トランスフォーム更新
	spawnTransform_.Update();

	// 出現エリア設定
	spawnAABBArea_.min_ = spawnTransform_.GetWorldPosition() - (spawnInfo_.GetData().size_ / 2.0f);
	spawnAABBArea_.max_ = spawnTransform_.GetWorldPosition() + (spawnInfo_.GetData().size_ / 2.0f);

#ifdef _DEBUG
	lineCommon_->GetDebugLineMeshData().AddLineAABB(AABB{-spawnInfo_.GetData().size_,spawnInfo_.GetData().size_}, spawnTransform_.GetWorldPosition());
#endif // _DEBUG

	// 出現
	SpawnProcess();
	
}

void CharacterSpawn::Draw() {

}

void CharacterSpawn::SpawnProcess(){
	if (spawnInfo_.IsEnd()) return;

	// 情報更新
	if (characterManager_->GetCharacterCount(CharacterType::Enemy) <= maxEnemyCount_) {
		spawnInfo_.Update(Engine::MyGame::GameTime());
	}

	// 出し切ったら
	if (spawnInfo_.IsSpawned()) return;
	spawnTransform_.Update();
	spawnAABBArea_.min_ = spawnTransform_.GetWorldPosition() - (spawnInfo_.GetData().size_ / 2.0f);
	spawnAABBArea_.max_ = spawnTransform_.GetWorldPosition() + (spawnInfo_.GetData().size_ / 2.0f);

	// グループ生成
	characterManager_->CreateEnemyGroup(spawnInfo_.GetData().type_, 0, spawnInfo_.GetData().spawnAmount_, spawnTransform_.GetWorldPosition(), spawnAABBArea_);

	// 出現した
	spawnInfo_.Spawned();
}

#include "CharacterSpawn.h"
#include"DirectXGame/application/base/Character/Base/CharacterManager.h"
#include"DirectXGame/engine/Line/LineCommon.h"
#include "DirectXGame/engine/MyGame/MyGame.h"

namespace Character {
	void CharacterSpawn::Initialize(CharacterManager* characterManager, Engine::LineCommon* line, const SpawnInfo& info) {
		this->characterManager = characterManager;	// キャラクター管理クラス
		this->lineCommon = line;						// ライン管理クラス

		spawnInfo_ = info;	// スポーン情報

		// トランスフォーム初期化
		spawnTransform_.Initialize();
		spawnTransform_.translate_ = spawnInfo_.GetData().translate_;
	}

	void CharacterSpawn::Update(float dt) {

		// トランスフォーム更新
		spawnTransform_.Update();

		// 出現エリア設定
		spawnAABBArea_.min = spawnTransform_.GetWorldPosition() - (spawnInfo_.GetData().size_ / 2.0f);
		spawnAABBArea_.max = spawnTransform_.GetWorldPosition() + (spawnInfo_.GetData().size_ / 2.0f);

#ifdef _DEBUG
		lineCommon->GetDebugLineMeshData().AddLineAABB(AABB{ -spawnInfo_.GetData().size_,spawnInfo_.GetData().size_ }, spawnTransform_.GetWorldPosition());
#endif // _DEBUG

		// 出現
		SpawnProcess();

	}

	void CharacterSpawn::Draw() {

	}

	void CharacterSpawn::SpawnProcess() {
		if (spawnInfo_.IsEnd()) return;

		// 情報更新
		if (characterManager->GetCharacterCount(Type::Enemy) <= maxEnemyCount_) {
			spawnInfo_.Update(Engine::MyGame::GameTime());
		}

		// 出し切ったら
		if (spawnInfo_.IsSpawned()) return;
		spawnTransform_.Update();
		spawnAABBArea_.min = spawnTransform_.GetWorldPosition() - (spawnInfo_.GetData().size_ / 2.0f);
		spawnAABBArea_.max = spawnTransform_.GetWorldPosition() + (spawnInfo_.GetData().size_ / 2.0f);

		// グループ生成。設定された群衆パターンは生成される全メンバーへ渡す
		characterManager->CreateEnemyGroup(
			spawnInfo_.GetData().type_,
			spawnInfo_.GetData().groupId_,
			spawnInfo_.GetData().spawnAmount_,
			spawnTransform_.GetWorldPosition(),
			spawnAABBArea_,
			spawnInfo_.GetData().crowdBehavior_
		);

		// 出現した
		spawnInfo_.Spawned();
	}
}

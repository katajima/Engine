#include "CharacterSpawnManager.h"

namespace Character {
	void CharacterSpawnManager::Initialize(CharacterManager* characterManager, Engine::LineCommon* line, int maxCharactorCount)
	{
		characterManager_ = characterManager;	// キャラクター管理
		lineCommon_ = line;						// ライン管理
		maxCharactorCount_ = maxCharactorCount;	// 最大存在数
	}


	void CharacterSpawnManager::Update(float dt) {

		// 更新
		for (auto& [name, spawn] : characterSpawns_) {
			spawn->Update(dt);
		}
	};

	void CharacterSpawnManager::AddCharacterSpawn(const SpawnInfo& info)
	{
		if (characterSpawns_.contains(info.GetData().name_)) { return; }

		// キャラクタースポーン位置追加
		std::unique_ptr<CharacterSpawn> spawn = std::make_unique<CharacterSpawn>();
		spawn->SetMaxEnemyCount(info.GetData().spawnMaxCount_);
		spawn->Initialize(characterManager_, lineCommon_, info);
		characterSpawns_[info.GetData().name_] = std::move(spawn);
	}

	CharacterSpawn* CharacterSpawnManager::GetCharacterSpawn(const std::string& name)
	{
		return characterSpawns_[name].get();
	}
}
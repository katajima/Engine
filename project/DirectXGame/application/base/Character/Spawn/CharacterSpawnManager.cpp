#include "CharacterSpawnManager.h"

void CharacterSpawnManager::Initialize(BaseCharacterManager* characterManager, LineCommon* line)
{
	characterManager_ = characterManager;
	lineCommon_ = line;
}


void CharacterSpawnManager::Update() {

	for (auto& [name, spawn] : characterSpawns_) {
		spawn->Update(0);
	}
};

void CharacterSpawnManager::AddCharacterSpawn(const SpawnInfo& info)
{
	if(characterSpawns_.contains(info.name_)){ return;}

	std::unique_ptr<CharacterSpawn> spawn = std::make_unique<CharacterSpawn>();
	spawn->Initialize(characterManager_, lineCommon_, info);
	characterSpawns_[info.name_] = std::move(spawn);
}

CharacterSpawn* CharacterSpawnManager::GetCharacterSpawn(const std::string& name)
{
	return characterSpawns_[name].get();
}

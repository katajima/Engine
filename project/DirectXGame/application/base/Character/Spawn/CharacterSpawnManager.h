#pragma once
#include "CharacterSpawn.h"
#include <map>

/// <summary>
/// キャラクターの出現情報を管理するマネージャー
/// </summary>
class CharacterSpawnManager {
public:
	/// <summary>
	///  初期化
	/// </summary>
	/// <param name="characterManager"></param>
	/// <param name="line"></param>
	void Initialize(BaseCharacterManager* characterManager, Engine::LineCommon* line);


	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// キャラクターのスポナーを追加
	/// </summary>
	void AddCharacterSpawn(const SpawnInfo& info);

	/// <summary>
	/// キャラクター出現エリアクラスを取得
	/// </summary>
	/// <param name="name"></param>
	/// <returns></returns>
	CharacterSpawn* GetCharacterSpawn(const std::string& name);



private:
	// キャラクター出現エリアコンテナ
	std::map<std::string, std::unique_ptr<CharacterSpawn>> characterSpawns_;

	// キャラクター最大出現量
	int maxCharactorCount_ = 100;


private:
	BaseCharacterManager* characterManager_ = nullptr;
	Engine::LineCommon* lineCommon_ = nullptr;
};
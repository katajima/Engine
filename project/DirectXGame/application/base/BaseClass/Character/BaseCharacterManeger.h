#pragma once

// engine
#include"DirectXGame/engine/Camera/Camera.h"
#include"DirectXGame/application/GlobalVariables/GlobalVariables.h"

// ベースキャラクター
#include"BaseCharacter.h"
// アプリケーション(敵)
#include"DirectXGame/application/base/BaseClass/Character/Enemy/BaseEnemy.h"

// アプリケーション(プレイヤー)
#include "DirectXGame/application/base/BaseClass/Character/Player/BasePlayer.h"

class Effect;
class FollowCamera;
class BulletManager;
class Entity3DManager;
class Entity2DManager;
class BaseCharacterManager
{
public:
	


	// 初期化
	void Initialize(Input* input ,Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, GlobalVariables* globalVariables, Camera* camera);

	// 更新
	void Update();

	void Draw2D();
public:
	void SetFollowCamera(FollowCamera* followCamera) { followCamera_ = followCamera; }
	void SetBulletManager(BulletManager* bulletManager) { bulletManager_ = bulletManager; }
	void SetEffect(Effect* effect) { effect_ = effect; }
public: // 取得系

	// キャラクターのタイプによっての数を取得
	int GetCharacterCount(CharacterType type) const
	{
		int result = 0;
		for (const auto& character : character_)
		{
			if (character->GetCharacterType() == type)
			{
				result++;
			}
		}

		return result;
	}

	// キャラクター
	std::vector<BaseCharacter*> GetCharacters() {
		std::vector<BaseCharacter*> result;
		for (auto& enemy : character_) {
			result.push_back(enemy.get());
		}
		return result;
	}

	// プレイヤー取得
	BasePlayer* GetPlayer() {
		for (auto& character : character_) {
			if (character->GetCharacterType() == CharacterType::Player) {
				return static_cast<BasePlayer*>(character.get());
			}
		}
		return nullptr;
	}

public: // 生成系
	// キャラクター生成(敵)
	void CreateCharacter(EnemyType enemyType, const std::string& characterName, Transform transform);
	// キャラクター生成(プレイヤー)
	void CreateCharacter(PlayerType playerType, const std::string& characterName, Transform transform);

private:
	
private:
	std::vector<std::unique_ptr<BaseCharacter>> character_;	// キャラクター

	uint32_t characterCount_ = 0;	// キャラクターの数

private: // 貰いもの
	Camera* camera_;
	Input* input_;
	Entity3DManager* entity3DManager_ = nullptr;
	Entity2DManager* entity2DManager_ = nullptr;
	GlobalVariables* globalVariables_;	// グローバル変数
	FollowCamera* followCamera_ = nullptr;	// フォローカメラ 
	BulletManager* bulletManager_ = nullptr; // 弾をマネジャー
	Effect* effect_ = nullptr;
};


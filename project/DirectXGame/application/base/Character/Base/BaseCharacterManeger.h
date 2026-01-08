#pragma once

// engine
#include"DirectXGame/engine/Camera/Camera.h"
#include"DirectXGame/application/GlobalVariables/GlobalVariables.h"

// ベースキャラクター
#include"BaseCharacter.h"
// アプリケーション(敵)
#include"DirectXGame/application/base/Character/Base/Enemy/BaseEnemy.h"

// アプリケーション(プレイヤー)
#include "DirectXGame/application/base/Character/Base/Player/BasePlayer.h"

#include "DirectXGame/application/base/Character/Crowd/CrowdManager.h"

//前方宣言
class Effect;
class FollowCamera;
class BulletManager;
class CameraManager;
class SpecalPointManager;

namespace Engine {
	class Entity3DManager;
	class Entity2DManager;
}


/// <summary>
/// キャラクターマネージャークラス
/// </summary>
class BaseCharacterManager
{
public:
	


	// 初期化
	void Initialize(Engine::Input* input , Engine::Entity3DManager* entity3DManager, Engine::Entity2DManager* entity2DManager, 
		Engine::GlobalVariables* globalVariables, Engine::Camera* camera);

	// 更新
	void Update();
	// 描画2d
	void Draw2D();
public:
	// カメラマネジャー設定
	void SetCameraManager(CameraManager* cameraManager) { cameraManager_ = cameraManager; };
	// フォローカメラ設定
	void SetFollowCamera(FollowCamera* followCamera) { followCamera_ = followCamera; }
	// 弾マネージャー設定
	void SetBulletManager(BulletManager* bulletManager) { bulletManager_ = bulletManager; }
	// エフェクト設定
	void SetEffect(Effect* effect) { effect_ = effect; }
	// スペシャルポイント管理クラスの設定
	void SetSpecialPointManager(SpecalPointManager* specalPointManager) { specalPointManager_ = specalPointManager; };
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


	/// <summary>
	/// 群衆AI取得
	/// </summary>
	/// <returns></returns>
	CrowdManager* GetCrowdManager() { return crowdManager_.get();}

public: // 生成系
	// キャラクター生成(敵)
	void CreateCharacter(EnemyType enemyType, const std::string& characterName,int groupId, Transform transform);
	// キャラクター生成(プレイヤー)
	void CreateCharacter(PlayerType playerType, const std::string& characterName, Transform transform);

	void CreateEnemyGroup(EnemyType enemyType,int groupCount, int perGroup, Vector3 origin, AABB aabb);

private:
	
private:
	std::vector<std::unique_ptr<BaseCharacter>> character_;	// キャラクター

	uint32_t characterCount_ = 0;	// キャラクターの数

	uint32_t enemyCount_ = 0;	// 敵カウンター

	std::unique_ptr<CrowdManager> crowdManager_;	// 群衆AI

private: // 貰いもの
	Engine::Camera* camera_;
	Engine::Input* input_;
	Engine::Entity3DManager* entity3DManager_ = nullptr;
	Engine::Entity2DManager* entity2DManager_ = nullptr;
	Engine::GlobalVariables* globalVariables_;			// グローバル変数
	FollowCamera* followCamera_ = nullptr;		// フォローカメラ 
	BulletManager* bulletManager_ = nullptr;	// 弾をマネジャー
	CameraManager* cameraManager_ = nullptr;	// カメラ管理
	Effect* effect_ = nullptr;
	SpecalPointManager* specalPointManager_ = nullptr;
};


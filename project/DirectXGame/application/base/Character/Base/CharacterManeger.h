#pragma once

// engine
#include"DirectXGame/engine/Camera/Camera.h"
#include"DirectXGame/application/GlobalVariables/GlobalVariables.h"

// ベースキャラクター
#include"BaseCharacter.h"
// アプリケーション(敵)
#include"DirectXGame/application/base/Character/Enemy/Base/BaseEnemy.h"
// アプリケーション(プレイヤー)
#include "DirectXGame/application/base/Character/Player/Base/BasePlayer.h"

#include <DirectXGame/application/base/Character/Enemy/Base/AI/EnemyAiSystem.h>
#include <DirectXGame/application/base/Character/Enemy/Base/AI/EnemyAiDebugSystem.h>
#include <DirectXGame/application/base/Character/Enemy/Base/AI/EnemyCrowdSpawnDebugSystem.h>

#include "CharacterGlobalData.h"

//前方宣言
class EffectSystem;
class FollowCamera;
class BulletManager;
class CameraManager;
class SpecalPointManager;

namespace Engine {
	class EntityManager;
}

namespace Character {
	/// <summary>
	/// キャラクターマネージャークラス
	/// </summary>
	class CharacterManager {
	public:
		// 初期化
		void Initialize(InputSystem* inputSystem,HitBox::System* hitBoxSystem,Engine::EntityManager* entity3DManager,
			Engine::GlobalVariables* globalVariables, Engine::Camera* camera);

		// 更新
		void Update(float dt,bool isMove);
		// 描画2d
		void Draw2D();
	public:
		// カメラマネジャー設定
		void SetCameraManager(CameraManager* cameraManager) { this->cameraManager = cameraManager; };
		// フォローカメラ設定
		void SetFollowCamera(FollowCamera* followCamera) { this->followCamera = followCamera; }
		// 弾マネージャー設定
		void SetBulletManager(BulletManager* bulletManager) { this->bulletManager = bulletManager; }
		// エフェクト設定
		void SetEffect(EffectSystem* effect) { this->effect = effect; }
		// スペシャルポイント管理クラスの設定
		void SetSpecialPointManager(SpecalPointManager* specalPointManager) { this->specalPointManager = specalPointManager; };
	public: // 取得系

		// キャラクターのタイプによっての数を取得
		int GetCharacterCount(Type type) const
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
				if (character->GetCharacterType() == Type::Player) {
					return static_cast<BasePlayer*>(character.get());
				}
			}
			return nullptr;
		}
		// 敵取得
		BaseEnemy* GetEnemy(uint32_t tagNumber) {
			for (auto& character : character_) {
				if (character->GetCharacterType() == Type::Enemy && character->GetTagNumber() == tagNumber) {
					return static_cast<BaseEnemy*>(character.get());
				}
			}
			return nullptr;
		}

	public: // 生成系
		// キャラクター生成(敵)
		int CreateCharacter(EnemyType enemyType, const std::string& characterName, int groupId, Transform transform,
			const CrowdBehaviorSettings& crowdBehavior = CrowdBehaviorSettings::Flocking());
		// キャラクター生成(プレイヤー)
		void CreateCharacter(PlayerType playerType, const std::string& characterName, Transform transform);

		void CreateEnemyGroup(EnemyType enemyType, int groupCount, int perGroup, Vector3 origin, AABB aabb,
			const CrowdBehaviorSettings& crowdBehavior = CrowdBehaviorSettings::Flocking());
	public:
		void Clear(Type type);
		// ウェーブ切り替え用に、敵を即削除せず退場演出へ移行させる
		void BeginEnemyWaveExit(float duration = 1.2f);
		// 指定した群衆グループだけを、退場演出を伴って削除する
		void BeginEnemyCrowdExit(int crowdGroupId, float duration = 1.2f);

		float GetScore() { return score; }
	private:
		// キャラクター
		std::vector<std::unique_ptr<BaseCharacter>> character_;	
		// キャラクターの数
		uint32_t characterCount_ = 0;	
		// 敵カウンター
		uint32_t enemyCount_ = 0;	
		// 敵AIシステム
		std::unique_ptr<EnemyAiSystem> enemyAiSystem_;	
		// 敵AIの判断内容を可視化するデバッグシステム
		std::unique_ptr<EnemyAiDebugSystem> enemyAiDebugSystem_;
		// 任意の群衆をImGuiから生成するデバッグシステム
		std::unique_ptr<EnemyCrowdSpawnDebugSystem> enemyCrowdSpawnDebugSystem_;

		float score = 0.0f;

		// キャラクター用保存項目データ
		std::map<std::string, GlobalData> globalDatas_;
	private: // 貰いもの
		Engine::Camera* camera = nullptr;
		// 入力システム
		InputSystem* inputSystem = nullptr;
		Engine::EntityManager* entityManager = nullptr;
		Engine::GlobalVariables* globalVariables = nullptr;	// グローバル変数
		FollowCamera* followCamera = nullptr;		// フォローカメラ 
		BulletManager* bulletManager = nullptr;	// 弾をマネジャー
		CameraManager* cameraManager = nullptr;	// カメラ管理
		EffectSystem* effect = nullptr;
		SpecalPointManager* specalPointManager = nullptr;
		HitBox::System* hitBoxSystem = nullptr;		// ヒットボックスシステム
	};
}

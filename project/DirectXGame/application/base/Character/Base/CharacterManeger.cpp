#include "CharacterManeger.h"
#include "DirectXGame/engine/MyGame/MyGame.h"
#include "DirectXGame/application/base/Character/Base/Characters.h"
#include <DirectXGame/application/base/Special/Point/SpecialPoint.h>
#include "DirectXGame/application/base/Character/State/CharacterStateMachine.h"
#include <DirectXGame/engine/Math/Random.h>

namespace Character {
	void CharacterManager::Initialize(InputSystem* inputSystem, HitBox::System* hitBoxSystem, Engine::EntityManager* entityManager,
		Engine::GlobalVariables* globalVariables, Engine::Camera* camera) {
		this->inputSystem = inputSystem;		// インプット
		this->entityManager = entityManager;	// エンティティ3d
		this->globalVariables = globalVariables; // 保存項目
		this->camera = camera;					// カメラ
		this->hitBoxSystem = hitBoxSystem;		// ヒットボックスシステム
		score = 0;
	}

	void CharacterManager::Update(bool isMove) {
		// 死亡したキャラクターを削除
		character_.erase(
			std::remove_if(character_.begin(), character_.end(),
				[this](const std::unique_ptr<BaseCharacter>& character) {
					if (!character) { return false; } 
					if (!character->GetAlive() && character->GetDelete()) {
						score++;
					}
					return !character->GetAlive() && character->GetDelete();
				}),
			character_.end());



		// プレイヤー座標をセット
		if (GetPlayer()) {
			specalPointManager->SetTarget(GetPlayer());
		}

		// キャラクター更新(敵)
		std::vector<const BaseCharacter*> target;
		for (auto& character : character_) {
			if (character) {
				if (character->GetCharacterType() == Type::Enemy) {
					character->IsMove(isMove);
					character->Update();
					if (character->GetAlive()) {
						target.push_back(character.get());
					}
				}
			}
		}

		// キャラクター更新(プレイヤー)
		if (GetPlayer()) {
			// ターゲット設定
			GetPlayer()->IsMove(isMove);
			GetPlayer()->SetTargetCharacters(target);
			GetPlayer()->Update();
		}
	}


	void CharacterManager::Draw2D() {
		// スプライト描画
		for (auto& character : character_) {
			if (character) {
				// 死んでいなければ
				if (character->GetCharacterStateMachine()->GetCurrentMainState() != CharacterMainState::Die) {
					if (character->GetAlive()) {
						character->Draw2D();
					}
				}
			}
		}
	}

	int CharacterManager::CreateCharacter(EnemyType enemyType, const std::string& characterName, int groupId, Transform transform){
		using EnemyFactory = std::function<std::unique_ptr<BaseEnemy>()>;

		// EnemyTypeから生成関数
		static const std::unordered_map<EnemyType, EnemyFactory> enemyFactoryMap =
		{
			{ EnemyType::kSmallMelee,   []() { return std::make_unique<SmallMeleeEnemy>(); } },
			{ EnemyType::kSmallRanged,   []() { return std::make_unique<SmallRangeEnemy>(); } },

			{ EnemyType::kMediumMelee,   []() { return std::make_unique<MediumMeleeEnemy>(); } },

			{ EnemyType::kDummy,   []() { return std::make_unique<DummyEnemy>(); } },
		};

		auto it = enemyFactoryMap.find(enemyType);
		assert(it != enemyFactoryMap.end() && "未対応のEnemyTypeです");

		std::unique_ptr<BaseEnemy> enemy = it->second();

		enemy->SetHitBoxSystem(hitBoxSystem);
		enemy->SetTagNumber(characterCount_);
		enemy->SetID(characterCount_);					// ID設定
		enemy->SetBulletManager(bulletManager);		// 弾管理クラス設定
		enemy->SetSpecalPointManager(specalPointManager);	// スペシャルポイント管理クラス設定
		enemy->SetEffect(effect);						// エフェクト設定
		enemy->Initialize(nullptr, entityManager, globalVariables, transform.translate, camera); // 初期化
		enemy->SetTargetCharacters(GetPlayer());					// ターゲット指定
		enemy->SetCharacterType(Type::Enemy);	// キャラクタータイプを敵に設定
		enemy->GetObjectComponent()->GetWorldTransform().translate_ = transform.translate;	// 位置指定
		enemy->GetObjectComponent()->GetWorldTransform().rotate_ = transform.rotate;		// 回転指定
		enemy->GetObjectComponent()->Update();	// ワールド行列更新
		enemy->GetObjectComponentShadow()->Update();	// ワールド行列更新
		character_.push_back(std::move(enemy));
		characterCount_++;
		return characterCount_ - 1; // 生成した敵のタグ番号を返す
	}

	void CharacterManager::CreateCharacter(PlayerType playerType, const std::string& characterName, Transform transform){
		std::unique_ptr<BasePlayer> player;

		player = std::make_unique<NormalPlayer>();
		player->SetHitBoxSystem(hitBoxSystem);
		player->SetTagNumber(characterCount_);
		player->SetFollowCamera(followCamera);		// フォローカメラ設定
		player->SetCameraManager(cameraManager);	// カメラ管理クラス設定
		player->SetBulletManager(bulletManager);	// 弾管理クラス設定
		player->SetSpecalPointManager(specalPointManager);	// スペシャルポイント管理クラス設定
		player->SetEffect(effect);					// エフェクト設定
		player->Initialize(inputSystem, entityManager, globalVariables, transform.translate, camera); // 初期化
		player->SetCharacterType(Type::Player);// キャラクターのタイプをプレイヤーに
		character_.push_back(std::move(player));	// キャラクターに追加 
		characterCount_++;
	}

	void CharacterManager::CreateEnemyGroup(EnemyType enemyType, int groupIds, int perGroup, Vector3 origin, AABB aabb) {
		// 敵を出現させる
		for (int i = 0; i < perGroup; ++i) {
			Vector3 pos = Random::RandomVector3(aabb.min, aabb.max);
			pos.y = 0.0f;
			CreateCharacter(enemyType, "enemy", {}, Transform{ {1,1,1}, {},pos });
		}
	}

	void CharacterManager::Clear(Type type) {
		for (auto& character : character_) {
			if (character->GetCharacterType() == type) {
				character->GetObjectComponent()->GetObjectStateFlags().isAlive = false;
				character->Delete();
				character->GetObjectComponent()->GetWorldTransform().scale_ = 0.0f;
				character->GetObjectComponentShadow()->GetWorldTransform().scale_ = 0.0f;;
			}
		}
	}

}


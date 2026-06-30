#include "CharacterManeger.h"
#include "DirectXGame/engine/MyGame/MyGame.h"
#include "DirectXGame/application/base/Character/Base/Characters.h"
#include <DirectXGame/application/base/Special/Point/SpecialPoint.h>
#include "DirectXGame/application/base/Character/State/CharacterStateMachine.h"
#include <DirectXGame/engine/Math/Random.h>
#include "DirectXGame/engine/Manager/Entity/EntityManager.h"

namespace Character {
	void CharacterManager::Initialize(InputSystem* inputSystem, HitBox::System* hitBoxSystem, Engine::EntityManager* entityManager,
		Engine::GlobalVariables* globalVariables, Engine::Camera* camera) {
		this->inputSystem = inputSystem;		// インプット
		this->entityManager = entityManager;	// エンティティ3d
		this->globalVariables = globalVariables; // 保存項目
		this->camera = camera;					// カメラ
		this->hitBoxSystem = hitBoxSystem;		// ヒットボックスシステム
		score = 0;

		// 敵AIシステムの初期化
		enemyAiSystem_ = std::make_unique<EnemyAiSystem>();
		enemyAiSystem_->Initialize();

		// AIデバッグ表示はライン描画基盤へ接続し、デバッグビルド時だけ表示を切り替える
		enemyAiDebugSystem_ = std::make_unique<EnemyAiDebugSystem>();
		enemyAiDebugSystem_->Initialize(entityManager->Get3DLineCommon());

		// テストしたい群衆構成をゲーム中に作れる生成デバッグ機能を用意する
		enemyCrowdSpawnDebugSystem_ = std::make_unique<EnemyCrowdSpawnDebugSystem>();
	}

	void CharacterManager::Update(float dt,bool isMove) {
		// 死亡したキャラクターを削除
		character_.erase(
			std::remove_if(character_.begin(), character_.end(),
				[this](const std::unique_ptr<BaseCharacter>& character) {
					if (!character) { return false; } 
					
					const bool isWaveExitEnemy = character->GetCharacterType() == Type::Enemy &&
						static_cast<BaseEnemy*>(character.get())->IsWaveExitRemoval();
					if (!character->GetAlive() && character->GetDelete() && !isWaveExitEnemy) {
						score++;
					}
					return !character->GetAlive() && character->GetDelete();
				}),
			character_.end());



		// プレイヤー削除後は敵が保持していた追跡ポインタを参照させない。
		// 死亡ステート中も追跡を止め、残った敵はゲーム終了まで待機させる。
		BasePlayer* player = GetPlayer();
		const bool hasEnemyTarget =
			player &&
			player->GetAlive() &&
			player->GetCharacterStateMachine()->GetCurrentMainState() != CharacterMainState::Die;

		// プレイヤー座標をセット
		if (player) {
			specalPointManager->SetTarget(player);
		}

		// キャラクター更新(敵)
		std::vector<BaseEnemy*> enemies;
		std::vector<BaseEnemy*> debugEnemies;
		std::vector<const BaseCharacter*> target;

		// 先に敵一覧だけ作る
		for (auto& character : character_) {
			if (!character) {
				continue;
			}

			if (character->GetCharacterType() == Type::Enemy) {
				BaseEnemy* enemy = static_cast<BaseEnemy*>(character.get());

				if (enemy->GetAlive() && !enemy->IsWaveExiting()) {
					debugEnemies.push_back(enemy);
					if (hasEnemyTarget) {
						// 有効なプレイヤーだけをロックオンへ再設定し、古い参照を使わせない
						enemy->SetTargetCharacters(player);
						target.push_back(enemy);
						enemies.push_back(enemy);
					}
				}
			}
		}

		// 敵一覧が入った後にスロット更新
		if (hasEnemyTarget) {
			enemyAiSystem_->UpdateSlot(
				enemies,
				player->GetWorldPosition(),
				player->GetObjectComponent()->GetWorldTransform().rotate_.y,
				dt
			);
		}

		// その後に敵更新
		for (BaseEnemy* enemy : enemies) {
			enemy->IsMove(isMove);
			enemy->Update();
		}

		// 退場中の敵はAI一覧から除外したうえで、退場演出のみ更新する
		for (auto& character : character_) {
			if (character && character->GetCharacterType() == Type::Enemy) {
				BaseEnemy* enemy = static_cast<BaseEnemy*>(character.get());
				if (enemy->GetAlive() && enemy->IsWaveExiting()) {
					enemy->Update();
				}
			}
		}

		// 攻撃要求許可
		enemyAiSystem_->UpdateRequest(enemies, dt);

		// 移動目標、役割、攻撃トークンを画面上へ可視化する
		enemyAiDebugSystem_->Update(debugEnemies, enemyAiSystem_.get());

		// 入力内容に応じた敵生成は更新ループの末尾で行い、次フレームからAIへ参加させる
		enemyCrowdSpawnDebugSystem_->Update(this);

		// キャラクター更新(プレイヤー)
		if (player) {
			// ターゲット設定
			player->IsMove(isMove);
			player->SetTargetCharacters(target);
			player->Update();
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

	uint32_t CharacterManager::CreateCharacter(EnemyType enemyType, const std::string& characterName, int groupId, Transform transform,
		const CrowdBehaviorSettings& crowdBehavior) {
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
		enemy->SetEnemyAiSystem(enemyAiSystem_.get());	// 敵AIシステム設定
		enemy->SetType(enemyType);						// 一覧表示や種類別処理に使用する敵種類を保持
		enemy->SetCrowdGroupId(groupId);					// 群衆グループ設定
		enemy->SetCrowdMemberIndex(enemyCount_++);			// 群衆内番号設定
		enemy->SetCrowdBehavior(crowdBehavior);				// 群衆の行動パターン設定
		enemy->Initialize(nullptr, entityManager, globalVariables, transform.translate, camera); // 初期化
		enemy->SetTargetCharacters(GetPlayer());					// ターゲット指定
		enemy->SetCharacterType(Type::Enemy);	// キャラクタータイプを敵に設定
		enemy->GetObjectComponent()->GetWorldTransform().translate_ = transform.translate;	// 位置指定
		enemy->GetObjectComponent()->GetWorldTransform().rotate_ = transform.rotate;		// 回転指定
		enemy->GetObjectComponent()->Update();	// ワールド行列更新
		character_.push_back(std::move(enemy));
		characterCount_++;
		return static_cast<uint32_t>(characterCount_ - 1); // 生成した敵のタグ番号を返す
	}

	uint32_t CharacterManager::CreateCharacter(PlayerType playerType, const std::string& characterName, Transform transform){
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
		return static_cast<uint32_t>(characterCount_ - 1); // 生成した敵のタグ番号を返す
	}

	void CharacterManager::CreateEnemyGroup(EnemyType enemyType, int groupIds, int perGroup, Vector3 origin, AABB aabb,
		const CrowdBehaviorSettings& crowdBehavior) {
		// 敵を出現させる
		for (int i = 0; i < perGroup; ++i) {
			Vector3 pos = Random::RandomVector3(aabb.min, aabb.max);
			// 高さは出現中心に固定し、ImGuiやイベントから指定した位置を反映する
			pos.y = origin.y;
			CreateCharacter(enemyType, "enemy", groupIds, Transform{ {1,1,1}, {},pos }, crowdBehavior);
		}
	}

	void CharacterManager::Clear(Type type) {
		for (auto& character : character_) {
			if (character->GetCharacterType() == type) {
				character->GetObjectComponent()->GetObjectStateFlags().isAlive = false;
				character->Delete();
				character->GetObjectComponent()->GetWorldTransform().scale_ = 0.0f;
			}
		}
	}

	void CharacterManager::BeginEnemyWaveExit(float duration) {
		for (auto& character : character_) {
			if (character && character->GetCharacterType() == Type::Enemy && character->GetAlive()) {
				BaseEnemy* enemy = static_cast<BaseEnemy*>(character.get());
				// すでに撃破演出へ入った敵は通常の得点と爆発演出を最後まで維持する
				if (enemy->GetCharacterStateMachine()->GetCurrentMainState() != CharacterMainState::Die) {
					enemy->BeginWaveExit(duration);
				}
			}
		}
	}

	void CharacterManager::BeginEnemyCrowdExit(int crowdGroupId, float duration) {
		for (auto& character : character_) {
			if (!character || character->GetCharacterType() != Type::Enemy || !character->GetAlive()) {
				continue;
			}

			BaseEnemy* enemy = static_cast<BaseEnemy*>(character.get());
			if (enemy->GetCrowdGroupId() != crowdGroupId || enemy->IsWaveExiting()) {
				continue;
			}

			// 撃破中の敵は既存の撃破演出を維持し、活動中の同一群衆だけを退場させる
			if (enemy->GetCharacterStateMachine()->GetCurrentMainState() != CharacterMainState::Die) {
				enemy->BeginWaveExit(duration);
			}
		}
	}

}


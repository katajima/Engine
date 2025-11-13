#include "BaseCharacterManeger.h"
#include "DirectXGame/engine/MyGame/MyGame.h"
#include "DirectXGame/application/base/Character/Base/Characters.h"

void BaseCharacterManager::Initialize(Input* input, Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, GlobalVariables* globalVariables, Camera* camera)
{
	input_ = input;						// インプット
	entity3DManager_ = entity3DManager;	// エンティティ3d
	entity2DManager_ = entity2DManager; // エンティティ2d
	globalVariables_ = globalVariables; // 保存項目
	camera_ = camera;					// カメラ

	// 群衆AI
	crowdManager_ = std::make_unique<CrowdManager>();
	crowdManager_->CreateGroup();
}

void BaseCharacterManager::Update()
{
	// 死亡したキャラクター(敵)を削除
	character_.erase(
		std::remove_if(character_.begin(), character_.end(),
			[](const std::unique_ptr<BaseCharacter>& character) {
				auto enemy = dynamic_cast<BaseEnemy*>(character.get());
				if (!enemy) { return false; } // 敵じゃない
				return !enemy->GetAlive() && enemy->GetDelete();
			}),
		character_.end());



	// プレイヤー座標をセット
	if (GetPlayer()) {
		crowdManager_->playerPos = GetPlayer()->GetObjectComponent()->GetWorldPosition();
	}

	// 群衆の結果を敵モデルに反映
	crowdManager_->UpdateAgentsToInstancing();

	// 群衆AI更新
	crowdManager_->Update(MyGame::GameTime());



	// キャラクター更新(敵)
	std::vector<BaseEnemy*> target;
	for (auto& character : character_) {
		if (character->GetCharacterType() == CharacterType::Enemy) {
			if (character) {
				character->Update();


				target.push_back(static_cast<BaseEnemy*>(character.get()));
			}
		}
	}

	// キャラクター更新(プレイヤー)
	if (GetPlayer()) {
		// ターゲット設定
		GetPlayer()->SetTargetCharacters(target);
		GetPlayer()->Update();
	}
}


void BaseCharacterManager::Draw2D()
{
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

void BaseCharacterManager::CreateCharacter(EnemyType enemyType, const std::string& characterName, int groupId, Transform transform)
{
	std::unique_ptr<BaseEnemy> enemy;
	// 敵タイプ
	switch (enemyType)
	{
	case EnemyType::kNormal: // 通常
		enemy = std::make_unique<NormalEnemy>();

		break;
	case EnemyType::kBullet:
		break;
	case EnemyType::kAttacker:
		break;
	case EnemyType::kTank:
		break;
	case EnemyType::kElite:
		break;
	case EnemyType::kBoss:
		break;
	default:

		break;
	}


	enemy->SetCharacterType(CharacterType::Enemy);	// キャラクタータイプを敵に設定
	enemy->SetID(characterCount_);					// ID設定
	enemy->SetPlayer(GetPlayer());					// ターゲット指定
	enemy->SetEffect(effect_);						// エフェクト設定
	enemy->Initialize(nullptr, entity3DManager_, entity2DManager_, globalVariables_, transform.translate, camera_); // 初期化
	enemy->GetObjectComponent()->GetWorldTransform().translate_ = transform.translate;	// 位置指定
	enemy->GetObjectComponent()->GetWorldTransform().rotate_ = transform.rotate;		// 回転指定


	// 群衆AI
	crowdManager_->CreateAgent(groupId, enemy.get(), transform.translate);

	character_.push_back(std::move(enemy));
}

void BaseCharacterManager::CreateCharacter(PlayerType playerType, const std::string& characterName, Transform transform)
{
	std::unique_ptr<BasePlayer> player;

	// プレイヤータイプ
	switch (playerType)
	{
	case PlayerType::kNormal:	// 通常(剣)
		player = std::make_unique<NormalPlayer>();
		break;
	case PlayerType::kBullet:	// 弾
		player = std::make_unique<BulletPlayer>();
		break;
	case PlayerType::kAttacker:
		break;
	case PlayerType::kTank:
		break;
	default:
		break;
	}
	
	player->SetCharacterType(CharacterType::Player);// キャラクターのタイプをプレイヤーに
	player->SetFollowCamera(followCamera_);		// フォローカメラ設定
	player->SetCameraManager(cameraManager_);	// カメラ管理クラス設定
	player->SetBulletManager(bulletManager_);	// 弾管理クラス設定
	player->SetEffect(effect_);					// エフェクト設定
	player->Initialize(input_, entity3DManager_, entity2DManager_, globalVariables_, transform.translate, camera_); // 初期化
	character_.push_back(std::move(player));	// キャラクターに追加 
}

void BaseCharacterManager::CreateEnemyGroup(int groupIds, int perGroup, Vector3 origin,AABB aabb)
{

	// グループId
	int groupId = crowdManager_->CreateGroup();
	crowdManager_->groups[groupId].Initialize(origin);

	// 敵を出現させる
	for (int i = 0; i < perGroup; ++i) {
		Vector3 pos = Random::RandomVector3(aabb.min_, aabb.max_);
		pos.y = 0.0f;
		CreateCharacter(EnemyType::kNormal, "enemy", groupId, Transform{ {1,1,1}, {},pos });
	}

	// 群衆管理クラスに追加
	std::vector<BaseEnemy*> enemys;
	for (auto& character : character_) {
		if (character->GetCharacterType() == CharacterType::Enemy) {
			enemys.push_back(static_cast<BaseEnemy*>(character.get()));
		}
	}

	crowdManager_->BindAgentsToEnemies(enemys);
}




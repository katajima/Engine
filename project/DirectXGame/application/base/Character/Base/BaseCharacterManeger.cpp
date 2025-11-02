#include "BaseCharacterManeger.h"
#include "DirectXGame/engine/MyGame/MyGame.h"
#include "DirectXGame/application/base/Character/Base/Characters.h"

void BaseCharacterManager::Initialize(Input* input, Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, GlobalVariables* globalVariables, Camera* camera)
{
	input_ = input;
	entity3DManager_ = entity3DManager;
	entity2DManager_ = entity2DManager;
	globalVariables_ = globalVariables;
	camera_ = camera;

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
		GetPlayer()->SetTargetCharacters(target);
		GetPlayer()->Update();
	}
}


void BaseCharacterManager::Draw2D()
{
	for (auto& character : character_) {
		if (character) {
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
	switch (enemyType)
	{
	case EnemyType::kNormal:
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


	enemy->SetCharacterType(CharacterType::Enemy);
	enemy->SetID(characterCount_);
	enemy->SetPlayer(GetPlayer());
	enemy->SetEffect(effect_);
	enemy->Initialize(nullptr, entity3DManager_, entity2DManager_, globalVariables_, transform.translate, camera_);
	enemy->GetObjectComponent()->GetWorldTransform().translate_ = transform.translate;
	enemy->GetObjectComponent()->GetWorldTransform().rotate_ = transform.rotate;


	// 群衆AI
	crowdManager_->CreateAgent(groupId, enemy.get(), transform.translate);

	character_.push_back(std::move(enemy));
}

void BaseCharacterManager::CreateCharacter(PlayerType playerType, const std::string& characterName, Transform transform)
{
	std::unique_ptr<BasePlayer> player;

	switch (playerType)
	{
	case PlayerType::kNormal:
		player = std::make_unique<NormalPlayer>();
		break;
	case PlayerType::kBullet:
		player = std::make_unique<BulletPlayer>();
		break;
	case PlayerType::kAttacker:
		break;
	case PlayerType::kTank:
		break;
	default:
		break;
	}
	player->SetCharacterType(CharacterType::Player);
	player->SetFollowCamera(followCamera_);
	player->SetBulletManager(bulletManager_);
	player->SetEffect(effect_);
	player->Initialize(input_, entity3DManager_, entity2DManager_, globalVariables_, transform.translate, camera_);
	character_.push_back(std::move(player));
}

void BaseCharacterManager::CreateEnemyGroup(int groupIds, int perGroup, Vector3 origin,AABB aabb)
{

	int groupId = crowdManager_->CreateGroup();
	crowdManager_->groups[groupId].Initialize(origin);

	for (int i = 0; i < perGroup; ++i) {
		Vector3 pos = Random::RandomVector3(aabb.min_, aabb.max_);
		pos.y = 0.0f;
		CreateCharacter(EnemyType::kNormal, "enemy", groupId, Transform{ {1,1,1}, {},pos });
	}

	std::vector<BaseEnemy*> enemys;
	for (auto& character : character_) {
		if (character->GetCharacterType() == CharacterType::Enemy) {
			enemys.push_back(static_cast<BaseEnemy*>(character.get()));
		}
	}

	crowdManager_->BindAgentsToEnemies(enemys);
}




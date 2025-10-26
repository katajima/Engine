#include "BaseCharacterManeger.h"

#include "DirectXGame/application/base/Character/Base/Characters.h"

void BaseCharacterManager::Initialize(Input* input, Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, GlobalVariables* globalVariables, Camera* camera)
{
	input_ = input;
	entity3DManager_ = entity3DManager;
	entity2DManager_ = entity2DManager;
	globalVariables_ = globalVariables;
	camera_ = camera;
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

void BaseCharacterManager::CreateCharacter(EnemyType enemyType, const std::string& characterName, Transform transform)
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




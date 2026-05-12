#include "BaseWeapon.h"
#include "DirectXGame/application/base/Object/ObjectComponent.h"
#include <DirectXGame/application/base/Attack/HitBox/HitBox.h>

BaseWeapon::BaseWeapon() = default;

BaseWeapon::~BaseWeapon() = default;
// 使っているキャラクター設定
void BaseWeapon::SetCharacter(Character::BaseCharacter* character) { this->character = character; }
// タグによるコライダーの有効・無効を設定
void BaseWeapon::SetIsCollider(CollisionTag tag, bool is) { GetObject3D()->GetColliderComponent()->SetEnableByTag(tag, is); };
// コライダーコンポーネント
Engine::ColliderComponent* BaseWeapon::GetColliderComponent() { return objectComponent_->GetColliderComponent(); };
// オブジェクト3d取得
Engine::Object3d* BaseWeapon::GetObject3D() { return objectComponent_->GetObject3D(); }
// ワールド変換取得
Engine::WorldTransform& BaseWeapon::GetWorldTransform() { return objectComponent_->GetWorldTransform(); }
// 弾管理クラス設定
void BaseWeapon::SetBulletManager(BulletManager* bulletManager) { this->bulletManager = bulletManager; }

RangedWeapon::~RangedWeapon() = default;

MeleeWeapon::~MeleeWeapon() = default;

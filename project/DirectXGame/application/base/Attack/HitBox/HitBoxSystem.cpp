#include "HitBoxSystem.h"


void HitBoxSystem::Initialize(BaseCharacter* character, Engine::Entity3DManager* entity3dManager) {
	character_ = character;
	entity3dManager_ = entity3dManager;
};

void HitBoxSystem::Update(float dt) {

	// 逆ループで安全に削除
	for (int i = (int)data_.size() - 1; i >= 0; i--) {

		Data& d = data_[i];

		// 更新
		d.hitBox->Update(dt);
		d.timer += dt;

		// 削除判定
		if (d.IsDelete()) {
			d.hitBox.reset();
			data_.erase(data_.begin() + i);
		}
	}

}

void HitBoxSystem::AddHitBox(HitBoxUseType type, const std::vector<HitBoxCollData>& datas, const std::vector<std::string>& useHitBoxName,
	float lifeTime, Type dependenceType, const Vector3& offset, Engine::WorldTransform* parent) {
	Data d;
	d.hitBox = std::make_unique<HitBox>();
	d.hitBox->Initialize(entity3dManager_, character_, type);
	d.hitBox->GetWorldTransform().Update();
	std::unique_ptr<Engine::OBBCollider> collObb = nullptr;
	std::unique_ptr<Engine::AABBCollider> collAABB = nullptr;
	std::unique_ptr<Engine::SphereCollider> collSphere = nullptr;
	Engine::WorldTransform world;
	world.Initialize();

	// 依存先設定
	switch (dependenceType)
	{
	case HitBoxSystem::Type::kParent: // 親子付け 
		d.hitBox->GetWorldTransform().parent_ = parent; // 親子設定
		break;
	case HitBoxSystem::Type::kIndependent: // 独立
		d.hitBox->GetWorldTransform().translate_ = parent->GetWorldPosition();	// ワールド座標に設定
		break;
	case HitBoxSystem::Type::kParentIndependent: // 追従先独立
	{
		world.parent_ = parent;	// ワールド座標に設定
		world.translate_ = offset;	// オフセット設定
		world.Update();

		d.hitBox->GetWorldTransform().translate_ = world.GetWorldPosition();
		break;
	}
	case HitBoxSystem::Type::kLockOnArea: // ターゲット位置へ(ターゲット位置のワールド座標を渡せば)
		d.hitBox->GetWorldTransform().translate_ = parent->GetWorldPosition();	// ワールド座標に設定
		break;
	default:
		break;
	}
	d.hitBox->GetWorldTransform().Update();

	

	for (auto& data : datas) {

		// ここで "名前が使われるコライダーかどうか" を判定する
		if (!useHitBoxName.empty()) {
			bool use = false;
			for (const auto& name : useHitBoxName) {
				if (data.name == name) {
					use = true;
					break;
				}
			}
			// 対象外ならスキップ
			if (!use) continue;
		}



		// 形状によっての設定項目
		switch (data.shape)
		{
		case HitBoxShape::kOBB:
			collObb = CreateCollider<Engine::OBBCollider>(data.tag, data.layer, data.mask, data.isEneble, data.isLine);
			collObb->obb.size = data.size;

			d.hitBox->AddCollider(std::move(collObb), data.offset, data.reactionData);
			break;
		case HitBoxShape::kAABB:
			collAABB = CreateCollider<Engine::AABBCollider>(data.tag, data.layer, data.mask, data.isEneble, data.isLine);
			collAABB->aabb.min_ = -data.size / 2;
			collAABB->aabb.max_ = data.size / 2;
			d.hitBox->AddCollider(std::move(collAABB), data.offset, data.reactionData);
			break;
		case HitBoxShape::kSphere:
			collSphere = CreateCollider<Engine::SphereCollider>(data.tag, data.layer, data.mask, data.isEneble, data.isLine);
			collSphere->radius = data.radius;
			d.hitBox->AddCollider(std::move(collSphere), data.offset, data.reactionData);
			break;
		default:
			break;
		}
	}



	d.lifeTime = lifeTime;							// 生存時間
	d.timer = 0.0f;									// 時間
	data_.push_back(std::move(d));
}

void HitBoxSystem::CreateHitBoxCollData(const std::string& name, HitBoxShape shape, HitBoxUseType useType,
	const GlobalHitBoxdata& hitBoxData, HitBoxCollData& data) {
	data.isEneble = true;	// 有効化
	data.isLine = true;		// ライン表示
	data.shape = shape;		// 形状選択
	data.name = name;		// 名前

	// コライダーの位置と大きさの設定
	data.offset = hitBoxData.offset;
	data.radius = hitBoxData.radius;
	data.size = hitBoxData.size;

	// 使用者
	switch (useType)
	{
	case HitBoxUseType::kPlayer:
		data.layer = CollisionLayer::PlayerAttack;
		data.tag = CollisionTag::PlayerAttack;
		data.mask = CollisionLayer::Enemy;
		break;
	case HitBoxUseType::kEnemy:
		data.layer = CollisionLayer::EnemyAttack;
		data.tag = CollisionTag::EnemyAttack;
		data.mask = CollisionLayer::Player;
		break;
	case HitBoxUseType::kOther:
		break;
	default:
		break;
	}
}


#include "HitBoxSystem.h"


void HitBoxSystem::Initialize(BaseCharacter* character, Entity3DManager* entity3dManager) {
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

void HitBoxSystem::AddHitBox(HitBoxUseType type, const std::vector<HitBoxCollData>& datas, const std::vector<std::string>& useHitBoxName, float lifeTime, WorldTransform* parent) {
	Data d;
	d.hitBox = std::make_unique<HitBox>();
	d.hitBox->Initialize(entity3dManager_, character_, type);

	std::unique_ptr<OBBCollider> collObb = nullptr;
	std::unique_ptr<AABBCollider> collAABB = nullptr;
	std::unique_ptr<SphereCollider> collSphere = nullptr;

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

		switch (data.shape)
		{
		case HitBoxShape::kOBB:
			collObb = CreateCollider<OBBCollider>(data.tag, data.layer, data.mask, data.isEneble, data.isLine);
			collObb->obb.size = data.size;
			d.hitBox->AddCollider(std::move(collObb), data.offset);
			break;
		case HitBoxShape::kAABB:
			collAABB = CreateCollider<AABBCollider>(data.tag, data.layer, data.mask, data.isEneble, data.isLine);
			collAABB->aabb.min_ = -data.size / 2;
			collAABB->aabb.max_ = data.size / 2;
			d.hitBox->AddCollider(std::move(collAABB), data.offset);
			break;
		case HitBoxShape::kSphere:
			collSphere = CreateCollider<SphereCollider>(data.tag, data.layer, data.mask, data.isEneble, data.isLine);
			collSphere->radius = data.radius;
			d.hitBox->AddCollider(std::move(collSphere), data.offset);
			break;
		default:
			break;
		}
	}
	d.hitBox->GetWorldTransform().parent_ = parent;


	d.lifeTime = lifeTime;
	d.timer = 0.0f;
	data_.push_back(std::move(d));
}

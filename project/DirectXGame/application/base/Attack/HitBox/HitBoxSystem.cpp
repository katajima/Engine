#include "HitBoxSystem.h"

namespace HitBox {
	void System::Initialize(BaseCharacter* character, Engine::Entity3DManager* entity3dManager) {
		character_ = character;
		entity3dManager_ = entity3dManager;
	};

	void System::Update(float dt) {

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

	void System::AddHitBox(UseType type, const std::vector<CollData>& datas, const std::vector<std::string>& useHitBoxName,
		float lifeTime, ParentType dependenceType, const Vector3& offset, Engine::WorldTransform* parent) {
		Data d;
		d.hitBox = std::make_unique<HitBoxInstance>();
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
		case ParentType::kParent: // 親子付け 
			d.hitBox->GetWorldTransform().parent_ = parent; // 親子設定
			break;
		case ParentType::kIndependent: // 独立
			d.hitBox->GetWorldTransform().translate_ = parent->GetWorldPosition();	// ワールド座標に設定
			break;
		case ParentType::kParentIndependent: // 追従先独立
		{
			world.parent_ = parent;	// ワールド座標に設定
			world.translate_ = offset;	// オフセット設定
			world.Update();

			d.hitBox->GetWorldTransform().translate_ = world.GetWorldPosition();
			break;
		}
		case ParentType::kLockOnArea: // ターゲット位置へ(ターゲット位置のワールド座標を渡せば)
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
			case Shape::kOBB:
				collObb = CreateCollider<Engine::OBBCollider>(data.tag, data.layer, data.mask, data.isEneble, data.isLine);
				collObb->obb.size = data.size;

				d.hitBox->AddCollider(std::move(collObb), data.offset, data.reactionData);
				break;
			case Shape::kAABB:
				collAABB = CreateCollider<Engine::AABBCollider>(data.tag, data.layer, data.mask, data.isEneble, data.isLine);
				collAABB->aabb.min_ = -data.size / 2;
				collAABB->aabb.max_ = data.size / 2;
				d.hitBox->AddCollider(std::move(collAABB), data.offset, data.reactionData);
				break;
			case Shape::kSphere:
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

	void System::CreateHitBoxCollData(const std::string& name, HitBox::Shape shape, UseType useType,
		const GlobalData& hitBoxData) {


		if (hitBoxCollDatas_.contains(name)) {
			return; // すでに存在する場合は何もしない
		}


		CollData data;
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
		case UseType::kPlayer:
			data.layer = CollisionLayer::PlayerAttack;
			data.tag = CollisionTag::PlayerAttack;
			data.mask = CollisionLayer::Enemy;
			break;
		case UseType::kEnemy:
			data.layer = CollisionLayer::EnemyAttack;
			data.tag = CollisionTag::EnemyAttack;
			data.mask = CollisionLayer::Player;
			break;
		case UseType::kOther:
			break;
		default:
			break;
		}
		hitBoxCollDatas_[name] = data;
	}
}

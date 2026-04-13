#include "HitBoxSystem.h"

namespace HitBox {
	void System::Initialize(Engine::EntityManager* entityManager) {
		this->entityManager = entityManager;
	};

	void System::Update(float dt) {


		// 期限付きヒットボックス更新
		// 逆ループで安全に削除
		for (int i = (int)lifeTimeHitBoxDatas_.size() - 1; i >= 0; i--) {
			Data& d = lifeTimeHitBoxDatas_[i];
			// 更新
			if (d.hitBox) {
				d.hitBox->Update(dt);
				d.timer += dt;
			}
			// 削除判定
			if (d.IsDelete()) {
				d.hitBox.reset();
				lifeTimeHitBoxDatas_.erase(lifeTimeHitBoxDatas_.begin() + i);
			}
		}
		// 無期限ヒットボックス更新
		for (auto& hit : hitBoxDatas_) {
			hit.hitBox->Update(dt);
		}
	}

	// ヒットボックス追加
	void System::AddLifeTimeHitBox(UseType type, Character::BaseCharacter* character, const std::vector<CollData>& datas, const std::vector<std::string>& useHitBoxName,
		float lifeTime, ParentType dependenceType, const Vector3& offset, bool useContactRecord, Engine::WorldTransform* parent) {
		Data d;
		d.hitBox = std::make_unique<HitBoxInstance>();
		d.hitBox->Initialize(entityManager, character, type, useContactRecord);
		d.hitBox->GetWorldTransform().Update();
		// 依存先設定
		CreateParent(d, dependenceType,offset,parent);
		// コライダー生成
		CreateHitBoxCollider(d, datas, useHitBoxName);
		d.lifeTime = lifeTime;							// 生存時間
		d.timer = 0.0f;									// 時間
		// ヒットボックス(期限付き)データに挿入
		lifeTimeHitBoxDatas_.push_back(std::move(d));
	}

	void System::AddHitBox(int32_t& id,UseType type, Character::BaseCharacter* character, const std::vector<CollData>& datas, const std::vector<std::string>& useHitBoxName,
		ParentType dependenceType, const Vector3& offset, bool useContactRecord, Engine::WorldTransform* parent) {
		
		// IDがあるなら早期リターン
		if (id != -1) return;
		Data d;
		d.hitBox = std::make_unique<HitBoxInstance>();
		d.hitBox->Initialize(entityManager, character, type, useContactRecord);
		d.hitBox->GetWorldTransform().Update();

		// Id設定;
		d.id = static_cast<int32_t>(hitBoxDatas_.size());
		id = d.id;

		// 依存先設定
		CreateParent(d, dependenceType, offset, parent);
		// コライダー生成
		CreateHitBoxCollider(d, datas, useHitBoxName);
		// ヒットボックス(無期限)データに挿入
		hitBoxDatas_.push_back(std::move(d));
	}

	// コライダーデータ生成
	void System::CreateHitBoxCollData(const std::string& name, HitBox::ShapeType shape, UseType useType,
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

	HitBoxInstance* System::GetHitBoxInstance(int32_t id) {
		for (auto& hit : hitBoxDatas_) {
			if (hit.id == id)
			return hit.hitBox.get();
		}
		return nullptr;
	}


	
	// 親子付け生成
	void System::CreateParent(Data& d, ParentType dependenceType, const Vector3& offset, Engine::WorldTransform* parent) {
		// 依存先設定
		switch (dependenceType){
		case ParentType::kParent: // 親子付け 
			d.hitBox->GetWorldTransform().parent_ = parent; // 親子設定
			break;
		case ParentType::kIndependent: // 独立
			d.hitBox->GetWorldTransform().translate_ = parent->GetWorldPosition();	// ワールド座標に設定
			break;
		case ParentType::kParentIndependent: // 追従先独立
		{
			Engine::WorldTransform world;
			world.Initialize();
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
	}
	// コライダー生成
	void System::CreateHitBoxCollider(Data& d, const std::vector<CollData>& datas, const std::vector<std::string>& useHitBoxName) {
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
			switch (data.shape){
			case ShapeType::kOBB:
			{
				std::unique_ptr<Engine::OBBCollider> collObb = nullptr;
				collObb = CreateCollider<Engine::OBBCollider>(data.tag, data.layer, data.mask, data.isEneble, data.isLine);
				collObb->obb.size = data.size;
				d.hitBox->AddCollider(std::move(collObb), data.offset, data.reactionData);
				break;
			}
			case ShapeType::kAABB:
			{
				std::unique_ptr<Engine::AABBCollider> collAABB = nullptr;
				collAABB = CreateCollider<Engine::AABBCollider>(data.tag, data.layer, data.mask, data.isEneble, data.isLine);
				collAABB->aabb.min = -data.size / 2;
				collAABB->aabb.max = data.size / 2;
				d.hitBox->AddCollider(std::move(collAABB), data.offset, data.reactionData);
				break;
			}
			case ShapeType::kSphere:
			{
				std::unique_ptr<Engine::SphereCollider> collSphere = nullptr;
				collSphere = CreateCollider<Engine::SphereCollider>(data.tag, data.layer, data.mask, data.isEneble, data.isLine);
				collSphere->radius = data.radius;
				d.hitBox->AddCollider(std::move(collSphere), data.offset, data.reactionData);
				break;
			}
			default:
				break;
			}
		}

	}
	// クリア
	void System::Clear() {
		for (auto& hit : lifeTimeHitBoxDatas_) {
			hit.hitBox.reset();
		}
		lifeTimeHitBoxDatas_.clear();
	}
}


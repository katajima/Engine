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
	void System::AddLifeTimeHitBox(Character::BaseCharacter* character, const CollData& datas,
		Engine::WorldTransform* parent, HitBoxOwnerId ownerId) {
		Data d;
		// 呼び出し側が指定しない場合も、個別判定として追跡できるIDを発行する。
		d.ownerId = ownerId == 0 ? AllocateOwnerId() : ownerId;
		d.hitBox = std::make_unique<HitBoxInstance>();
		d.hitBox->Initialize(entityManager, character, datas.hitBoxData.useType, datas.hitBoxData.useContactRecord);
		d.hitBox->SetRecordPerCollider(datas.hitBoxData.recordPerCollider);
		d.hitBox->GetWorldTransform().Update();
		// 依存先設定
		CreateParent(d, datas.hitBoxData.dependenceType, datas.hitBoxData.offset, parent);
		d.hitBox->SetTrajectory(datas.hitBoxData, parent);
		// コライダー生成
		CreateHitBoxCollider(d, datas);
		d.lifeTime = datas.hitBoxData.lifeTime;							// 生存時間
		d.timer = 0.0f;									// 時間
		// ヒットボックス(期限付き)データに挿入
		lifeTimeHitBoxDatas_.push_back(std::move(d));
	}

	HitBoxOwnerId System::AllocateOwnerId() {
		// 0は未指定を表す予約値なので、実IDには使用しない。
		if (nextOwnerId_ == 0) {
			nextOwnerId_ = 1;
		}
		return nextOwnerId_++;
	}

	void System::AddHitBox(int32_t& id, Character::BaseCharacter* character, const CollData& datas,
		 Engine::WorldTransform* parent) {

		// IDがあるなら早期リターン
		if (id != -1) return;
		Data d;
		d.hitBox = std::make_unique<HitBoxInstance>();
		d.hitBox->Initialize(entityManager, character, datas.hitBoxData.useType, datas.hitBoxData.useContactRecord);
		d.hitBox->SetRecordPerCollider(datas.hitBoxData.recordPerCollider);
		d.hitBox->GetWorldTransform().Update();

		// Id設定;
		d.id = static_cast<int32_t>(hitBoxDatas_.size());
		id = d.id;

		// 依存先設定
		CreateParent(d, datas.hitBoxData.dependenceType, datas.hitBoxData.offset, parent);
		d.hitBox->SetTrajectory(datas.hitBoxData, parent);
		// コライダー生成
		CreateHitBoxCollider(d, datas);
		// ヒットボックス(無期限)データに挿入
		hitBoxDatas_.push_back(std::move(d));
	}
	//
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
		switch (dependenceType) {
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
	void System::CreateHitBoxCollider(Data& d, const CollData& data) {
		CollisionTag tag = data.hitBoxData.tag;
		CollisionLayer layer = data.hitBoxData.layer;
		CollisionLayer mask = data.hitBoxData.mask;
		bool isEneble = data.hitBoxData.isEneble;
		bool isLine = data.hitBoxData.isLine;

		// 形状によっての設定項目
		switch (data.hitBoxData.shapeType) {
		case ShapeType::kOBB:
		{
			std::unique_ptr<Engine::OBBCollider> collObb = nullptr;
			collObb = CreateCollider<Engine::OBBCollider>(tag, layer, mask, isEneble, isLine);
			collObb->obb.size = data.hitBoxData.colliderSize;
			d.hitBox->AddCollider(std::move(collObb), data.hitBoxData.offset, data.reactionData);
			break;
		}
		case ShapeType::kAABB:
		{
			std::unique_ptr<Engine::AABBCollider> collAABB = nullptr;
			collAABB = CreateCollider<Engine::AABBCollider>(tag, layer, mask, isEneble, isLine);
			collAABB->aabb.min = -data.hitBoxData.colliderSize / 2;
			collAABB->aabb.max = data.hitBoxData.colliderSize / 2;
			d.hitBox->AddCollider(std::move(collAABB), data.hitBoxData.offset, data.reactionData);
			break;
		}
		case ShapeType::kSphere:
		{
			std::unique_ptr<Engine::SphereCollider> collSphere = nullptr;
			collSphere = CreateCollider<Engine::SphereCollider>(tag, layer, mask, isEneble, isLine);
			collSphere->radius = data.hitBoxData.radius;
			d.hitBox->AddCollider(std::move(collSphere), data.hitBoxData.offset, data.reactionData);
			break;
		}
		default:
			break;
		}
	}
	// 一時判定だけをクリアし、無期限判定の再利用状態を維持する
	void System::ClearLifeTimeHitBoxes() {
		for (auto& hit : lifeTimeHitBoxDatas_) {
			// 一時判定のコライダーを先に破棄して登録を解除する
			hit.hitBox.reset();
		}
		lifeTimeHitBoxDatas_.clear();
	}

	void System::ClearLifeTimeHitBoxes(HitBoxOwnerId ownerId) {
		// 指定グループ以外の一時判定を残し、終了したアクションだけを解放する。
		if (ownerId == 0) {
			return;
		}
		lifeTimeHitBoxDatas_.erase(
			std::remove_if(lifeTimeHitBoxDatas_.begin(), lifeTimeHitBoxDatas_.end(),
				[ownerId](const Data& data) { return data.ownerId == ownerId; }),
			lifeTimeHitBoxDatas_.end());
	}

	// 一時判定と無期限判定を含む全ヒットボックスを解放する
	void System::Clear() {
		// 一時判定は専用処理で確実に破棄する
		ClearLifeTimeHitBoxes();
		// 無期限判定はシステム破棄や再初期化時にだけ全解放する
		hitBoxDatas_.clear();
	}
}


#pragma once
#include "HitBox.h"
#include <DirectXGame/application/base/Attack/AttackData.h>
#include <DirectXGame/application/base/Attack/Combo/Base/ComboGlobalData.h>
#include "HitBoxEditor.h"

namespace Engine {
	class EntityManager; // 前方宣言
	class GlobalVariables;
}
namespace Character {
	class BaseCharacter;
}
namespace HitBox {
	/// <summary>
	/// 当たり判定を管理するシステム
	/// </summary>
	class System {
	public:
		~System() {
			lifeTimeHitBoxDatas_.clear();
			hitBoxCollDatas_.clear();
		}

		struct Data {
			std::unique_ptr<HitBoxInstance> hitBox = nullptr;
			int32_t id = 0;
			float lifeTime = 0.0f;
			float timer = 0.0f;
			// 生存時間を過ぎたら削除
			bool IsDelete() const { return timer > lifeTime; }
		};


		// 初期化
		void Initialize(Engine::EntityManager* entityManager);
		// 更新
		void Update(float dt);


		// ヒットボックス追加（期限付き）
		void AddLifeTimeHitBox(UseType type, Character::BaseCharacter* character, const std::vector<CollData>& datas, const std::vector<std::string>& useHitBoxName
			, float lifeTime, ParentType dependenceType, const Vector3& offset,bool useContactRecord,Engine::WorldTransform* parent = nullptr);

		// ヒットボックス追加（無期限）
		void AddHitBox(int32_t& id,UseType type, Character::BaseCharacter* character, const std::vector<CollData>& datas, const std::vector<std::string>& useHitBoxName
			, ParentType dependenceType, const Vector3& offset, bool useContactRecord, Engine::WorldTransform* parent = nullptr);


		// ヒットボックスコライダーデータ作成
		void CreateHitBoxCollData(const std::string& name, ShapeType shape, UseType useType, const GlobalData& hitBoxData);

		// 全体データ取得(期限付きヒットボックス)
		std::vector<Data>& GetLifeTimeHitBoxData() { return lifeTimeHitBoxDatas_; }
		// 全体データ取得(無期限ヒットボックス)
		std::vector<Data>& GetHitBoxData() { return hitBoxDatas_; }
		// ヒットボックスインスタンス取得
		HitBoxInstance* GetHitBoxInstance(int32_t id);

		// 名前からヒットボックスコライダーデータ取得
		CollData GetHitBoxCollData(const std::string& name) {
			if (hitBoxCollDatas_.find(name) != hitBoxCollDatas_.end()) {
				return hitBoxCollDatas_[name];
			}
			else {
				// 見つからなかった場合、空のデータを返す
				return CollData{};
			}
		}

		void Clear();

	private:
		// 親子付け生成処理
		void CreateParent(Data& d, ParentType dependenceType, const Vector3& offset, Engine::WorldTransform* parent);

		// コライダーの生成処理
		void CreateHitBoxCollider(Data& d, const std::vector<CollData>& datas, const std::vector<std::string>& useHitBoxName);
		// コライダー生成
		template <typename T>
		static std::unique_ptr<T> CreateCollider(CollisionTag tag, CollisionLayer layer, CollisionLayer mask, bool isEneble = true, bool isLine = false);
	private:
		// 期限付きヒットボックスデータ
		std::vector<Data> lifeTimeHitBoxDatas_;
		// 無期限ヒットボックスデータ
		std::vector<Data> hitBoxDatas_;
		// 無期限ヒットボックス用カウント

		// ヒットボックスコライダーデータ群
		std::map<std::string, CollData> hitBoxCollDatas_;
	private:
		Engine::EntityManager* entityManager = nullptr;
	};


	template <typename T>
	static std::unique_ptr<T> System::CreateCollider(CollisionTag tag, CollisionLayer layer, CollisionLayer mask, bool isEneble, bool isLine)
	{
		std::unique_ptr<T> coll = std::make_unique<T>();

		// 有効化
		if (isEneble) {
			coll->Enable();
		}
		else {
			coll->Disable();
		}
		// デバック用表示
		if (isLine) {
			coll->isDebugLine = true;
		}

		coll->tag = tag;			// タグ設定
		coll->layer = layer;		// レイヤー設定
		coll->collisionMask = (1 << static_cast<uint32_t>(mask));	// マスク設定

		return std::move(coll);
	}
}
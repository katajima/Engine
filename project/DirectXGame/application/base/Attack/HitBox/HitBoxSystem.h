#pragma once
#include "HitBox.h"
#include <DirectXGame/application/base/Attack/Combo/Base/ComboGlobalData.h>

namespace Engine {
	class EntityManager; // 前方宣言
	class GlobalVariables;
	class WorldTransform;
}

namespace HitBox {
	/// <summary>
	/// 当たり判定を管理するシステム
	/// </summary>
	class System {
	public:
		~System() {
			lifeTimeHitBoxDatas_.clear();
		}

		struct Data {
			std::unique_ptr<HitBoxInstance> hitBox = nullptr;
			int32_t id = 0;
			float lifeTime = 0.0f;
			float timer = 0.0f;
			/// <summary>
			/// 生存時間を過ぎたら削除
			/// </summary>
			bool IsDelete() const { return timer > lifeTime; }
		};


		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize(Engine::EntityManager* entityManager);
		/// <summary>
		/// 更新
		/// </summary>
		void Update(float dt);


		// ヒットボックス追加（期限付き）
		void AddLifeTimeHitBox(Character::BaseCharacter* character,const CollData& datas,
			Engine::WorldTransform* parent = nullptr);

		// ヒットボックス追加（無期限）
		void AddHitBox(int32_t& id,Character::BaseCharacter* character,const CollData& datas,
			Engine::WorldTransform* parent = nullptr);
		/// <summary>
		/// 全体データ取得(期限付きヒットボックス)
		/// </summary>
		std::vector<Data>& GetLifeTimeHitBoxData() { return lifeTimeHitBoxDatas_; }
		/// <summary>
		/// 全体データ取得(無期限ヒットボックス)
		/// </summary>
		std::vector<Data>& GetHitBoxData() { return hitBoxDatas_; }
		/// <summary>
		/// ヒットボックスインスタンス取得
		/// </summary>
		HitBoxInstance* GetHitBoxInstance(int32_t id);
		/// <summary>
		/// クリア
		/// </summary>
		void Clear();

	private:
		/// <summary>
		/// 親子付け生成処理
		/// </summary>
		void CreateParent(Data& d, ParentType dependenceType, const Vector3& offset, Engine::WorldTransform* parent);

		/// <summary>
		/// コライダーの生成処理
		/// </summary>
		void CreateHitBoxCollider(Data& d,const CollData& datas);
		// コライダー生成
		template <typename T>
		static std::unique_ptr<T> CreateCollider(CollisionTag tag, CollisionLayer layer, CollisionLayer mask, bool isEneble = true, bool isLine = false);
	private:
		// 期限付きヒットボックスデータ
		std::vector<Data> lifeTimeHitBoxDatas_;
		// 無期限ヒットボックスデータ
		std::vector<Data> hitBoxDatas_;
	private:
		Engine::EntityManager* entityManager = nullptr;
	};


	template <typename T>
	static std::unique_ptr<T> System::CreateCollider(CollisionTag tag, CollisionLayer layer, CollisionLayer mask, bool isEneble, bool isLine)
	{
		std::unique_ptr<T> coll = std::make_unique<T>();

		/// <summary>
		/// 有効化
		/// </summary>
		if (isEneble) {
			coll->Enable();
		}
		else {
			coll->Disable();
		}
		/// <summary>
		/// デバック用表示
		/// </summary>
		if (isLine) {
			coll->SetIsDebugLine(true);
		}

		coll->SetTag(tag);			// タグ設定
		coll->SetLayer(layer);		// レイヤー設定
		coll->SetCollisionMask(static_cast<uint32_t>(mask));	// マスク設定

		return std::move(coll);
	}
}

#pragma once
#include "HitBox.h"
#include <algorithm>
#include <DirectXGame/application/base/Attack/Combo/Base/ComboGlobalData.h>
#include <cstdint>

namespace Engine {
	class EntityManager; // 前方宣言
	class GlobalVariables;
	class WorldTransform;
}

namespace HitBox {
	// 期限付きヒットボックスを発生させた実行単位を識別するIDです。
	using HitBoxOwnerId = std::uint64_t;

	/// <summary>
	/// 当たり判定を管理するシステム
	/// </summary>
	class System {
	public:
		~System() {
			// システム破棄時は一時判定と無期限判定をまとめて解放する
			Clear();
		}

		struct Data {
			std::unique_ptr<HitBoxInstance> hitBox = nullptr;
			int32_t id = 0;
			// 同じキャラクター上の別アクションを混同しないための所有者IDです。
			HitBoxOwnerId ownerId = 0;
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


		/// <summary>期限付きヒットボックスを追加します。</summary>
		/// <param name="character">ヒットボックスの所有キャラクターです。</param>
		/// <param name="datas">生成する形状と寿命の設定です。</param>
		/// <param name="parent">親トランスフォームです。</param>
		/// <param name="ownerId">この判定グループの所有者IDです。</param>
		void AddLifeTimeHitBox(Character::BaseCharacter* character,const CollData& datas,
			Engine::WorldTransform* parent = nullptr, HitBoxOwnerId ownerId = 0);

		/// <summary>新しい期限付き判定グループ用の所有者IDを発行します。</summary>
		/// <returns>このシステム内で一意な所有者IDです。</returns>
		HitBoxOwnerId AllocateOwnerId();

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
		/// 一時判定だけをクリアし、再利用する無期限判定は保持する。
		/// </summary>
		void ClearLifeTimeHitBoxes();
		/// <summary>指定した所有者の期限付き判定だけを解放します。</summary>
		/// <param name="ownerId">解放対象の所有者IDです。</param>
		void ClearLifeTimeHitBoxes(HitBoxOwnerId ownerId);
		/// <summary>
		/// 一時判定と無期限判定を含む全ヒットボックスを解放する。
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
		// 0を未指定値として予約し、実際のIDには使用しない連番です。
		HitBoxOwnerId nextOwnerId_ = 1;
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

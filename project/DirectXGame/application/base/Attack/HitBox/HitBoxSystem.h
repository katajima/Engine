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
			data_.clear();
			hitBoxCollDatas_.clear();
		}

		struct Data {
			std::unique_ptr<HitBoxInstance> hitBox = nullptr;
			float lifeTime = 0.0f;
			float timer = 0.0f;
			// 生存時間を過ぎたら削除
			bool IsDelete() const { return timer > lifeTime; }
		};


		// 初期化
		void Initialize(Character::BaseCharacter* character, Engine::EntityManager* entityManager);
		// 更新
		void Update(float dt);


		// ヒットボックス追加
		void AddHitBox(UseType type, const std::vector<CollData>& datas, const std::vector<std::string>& useHitBoxName
			,float lifeTime, ParentType dependenceType, const Vector3& offset, Engine::WorldTransform* parent = nullptr);


		// ヒットボックスコライダーデータ作成
		void CreateHitBoxCollData(const std::string& name, Shape shape, UseType useType, const GlobalData& hitBoxData);

		// 全体データ取得
		std::vector<Data>& GetData() { return data_; }

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
		std::vector<Data> data_;

		// ヒットボックスコライダーデータ群
		std::map<std::string, CollData> hitBoxCollDatas_;
	private:
		Character::BaseCharacter* character = nullptr;
		Engine::EntityManager* entityManager = nullptr;
	};
}
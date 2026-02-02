#pragma once
#include "HitBox.h"
#include <DirectXGame/application/base/Attack/AttackData.h>
#include <DirectXGame/application/base/Attack/Combo/Base/ComboGlobalData.h>
#include "HitBoxEditor.h"

namespace Engine {
	class Entity3DManager; // 前方宣言
	class GlobalVariables;
}
class BaseCharacter;

/// <summary>
/// 当たり判定を管理するシステム
/// </summary>
class HitBoxSystem {
public:
	
	struct Data {
		std::unique_ptr<HitBox> hitBox = nullptr;
		float lifeTime = 0.0f;
		float timer = 0.0f;
		// 生存時間を過ぎたら削除
		bool IsDelete() const { return timer > lifeTime; }
	};


	// 初期化
	void Initialize(BaseCharacter* character , Engine::Entity3DManager* entity3dManager);
	// 更新
	void Update(float dt);


	// ヒットボックス追加
	void AddHitBox(HitBoxUseType type, const std::vector<HitBoxCollData>& datas,const std::vector<std::string>& useHitBoxName 
		,float lifeTime, HitBoxParentType dependenceType ,const Vector3& offset , Engine::WorldTransform* parent = nullptr);


	// ヒットボックスコライダーデータ作成
	void CreateHitBoxCollData(const std::string& name, HitBoxShape shape, HitBoxUseType useType,const GlobalHitBoxdata& hitBoxData);

	// 全体データ取得
	std::vector<Data>& GetData() {return data_;}

	// 名前からヒットボックスコライダーデータ取得
	HitBoxCollData GetHitBoxCollData(const std::string& name) {
		if (hitBoxCollDatas_.find(name) != hitBoxCollDatas_.end()) {
			return hitBoxCollDatas_[name];
		}
		else {
			// 見つからなかった場合、空のデータを返す
			return HitBoxCollData{};
		}
	}



public: // 保存や適応に関しての関数

	// 保存項目の追加
	void ApplyGlobalComboData(const std::string& name, ComboGlovalData& data);

	// 保存項目の適応
	void GetGlobalComboData(const std::string& name, ComboGlovalData& data);

	// 保存項目に設定
	void SetGlobalComboData(const std::string& name, ComboGlovalData& data);
private:
	std::vector<Data> data_;

	// ヒットボックスコライダーデータ群
	std::map<std::string, HitBoxCollData> hitBoxCollDatas_;
private:
	BaseCharacter* character_ = nullptr;
	Engine::Entity3DManager* entity3dManager_ = nullptr;
};
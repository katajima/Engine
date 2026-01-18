#pragma once
#include "HitBox.h"
#include <DirectXGame/application/base/Attack/AttackData.h>
#include <DirectXGame/application/base/Attack/Combo/Base/ComboGlobalData.h>

namespace Engine {
	class Entity3DManager; // 前方宣言
}
class BaseCharacter;

/// <summary>
/// 当たり判定を管理するシステム
/// </summary>
class HitBoxSystem {
public:
	// ヒットボックスのタイプ
	enum class Type {
		kParent,			// 追従型
		kIndependent,		// 独立(飛び道具や範囲攻撃など)
		kParentIndependent,	// 追従位置での独立(武器の中間位置で発生してその後独立)

		kLockOnArea,	// ターゲット位置
	};


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
		,float lifeTime, Type dependenceType ,const Vector3& offset , Engine::WorldTransform* parent = nullptr);


	void CreateHitBoxCollData(const std::string& name, HitBoxShape shape, HitBoxUseType useType,const GlobalHitBoxdata& hitBoxData ,HitBoxCollData& data);

	// 全体データ取得
	std::vector<Data>& GetData() {return data_;}

private:
	std::vector<Data> data_;

private:
	BaseCharacter* character_ = nullptr;
	Engine::Entity3DManager* entity3dManager_ = nullptr;
};
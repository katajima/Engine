#pragma once
#include "HitBox.h"

class Entity3DManager; // 前方宣言
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
	void Initialize(BaseCharacter* character , Entity3DManager* entity3dManager);
	// 更新
	void Update(float dt);


	// ヒットボックス追加
	void AddHitBox(HitBoxUseType type, const std::vector<HitBoxCollData>& datas,const std::vector<std::string>& useHitBoxName ,float lifeTime,WorldTransform* parent = nullptr);

	// 全体データ取得
	std::vector<Data>& GetData() {return data_;}

private:
	std::vector<Data> data_;

private:
	BaseCharacter* character_ = nullptr;
	Entity3DManager* entity3dManager_ = nullptr;
};
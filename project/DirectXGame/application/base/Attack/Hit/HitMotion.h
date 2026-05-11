#pragma once
#include <DirectXGame/application/base/Attack/AttackData.h>

class ObjectComponent; // 前方宣言

/// <summary>
/// ダメージモーション
/// </summary>
class DamageMotion {
public:

	// 更新
	void Update(float dt);
	// ダメージデータ設定
	void SetData(const DamageData& data) {
		damageData_ = data;
		isPlaying_ = true;
	}
	// ダメージデータ取得
	DamageData GetDamageData() const { return damageData_; }
	// 再生中か取得
	bool IsPlaying() const { return isPlaying_; }

private:
	bool isPlaying_ = false;
	DamageData damageData_;
};


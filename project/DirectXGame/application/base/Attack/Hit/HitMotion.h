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
	void SetData(const DamageDataHandle& data) {
		damageData_ = data;
		isPlaying_ = true;
	}
	// ダメージデータ取得
	const DamageDataHandle& GetDamageData() const { return damageData_; }
	// 再生中か取得
	bool IsPlaying() const { return isPlaying_; }

private:
	bool isPlaying_ = false;
	DamageDataHandle damageData_;	// 派生ダメージデータを所有するコピー可能なハンドル
};


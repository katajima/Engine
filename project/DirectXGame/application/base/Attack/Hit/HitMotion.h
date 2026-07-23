#pragma once
#include <DirectXGame/application/base/Attack/AttackData.h>

class ObjectComponent; // 前方宣言

/// <summary>
/// ダメージモーション
/// </summary>
class DamageMotion {
public:

	/// <summary>
	/// 更新
	/// </summary>
	void Update(float dt);
	/// <summary>
	/// ダメージデータ設定
	/// </summary>
	void SetData(const DamageDataHandle& data) {
		damageData_ = data;
		isPlaying_ = true;
	}
	/// <summary>
	/// ダメージデータ取得
	/// </summary>
	const DamageDataHandle& GetDamageData() const { return damageData_; }
	/// <summary>
	/// 再生中か取得
	/// </summary>
	bool IsPlaying() const { return isPlaying_; }

private:
	bool isPlaying_ = false;
	DamageDataHandle damageData_;	// 派生ダメージデータを所有するコピー可能なハンドル
};


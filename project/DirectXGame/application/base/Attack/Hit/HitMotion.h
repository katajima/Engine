#pragma once
#include "DirectXGame/engine/struct/Vector3.h"
#include <DirectXGame/application/base/Attack/AttackData.h>

class ObjectComponent; // 前方宣言

// ヒットストップのモーション
class HitStopMotion{
public:

	
	// 更新
	void Update(float dt);

	// 地面に着地しているか設定
	void SetIsGround(bool is) { isGround_ = is; };

	// ヒットストップデータ取得
	HitStopData GetData() const { return hitStopData_; }

	// ヒットストップデータ設定
	void SetData(const HitStopData& data) { 
		hitStopData_ = data; 
		isPlaying_ = true;
	}

	// 揺れ幅取得
	Vector3 Vibration() const {
		return hitStopData_.GetData().Vibration(isGround_);
	};

	// 再生中か取得
	bool IsPlaying() const { return isPlaying_; }

private:
	bool isHit_ = false;	// 衝突したか
	bool isGround_ = true;	// 地面についているか

	bool isPlaying_ = false;
	HitStopData hitStopData_;	// ヒットストップデータ
};

// ノックバックのモーション
class KnockbackMotion {
public:

	// 更新
	void Update(float dt, ObjectComponent* object);

	// ノックバックデータ設定
	void SetData(const KnockbackData& data) { 
		knockbackData_ = data; 
		isPlaying_ = true;
	}

	// ノックバックデータ取得
	KnockbackData GetData() const { return knockbackData_; }

	// 再生中か取得
	bool IsPlaying() const { return isPlaying_; }

private:
	bool isPlaying_ = false;
	KnockbackData knockbackData_;	// ノックバックデータ
};

/// <summary>
/// 吸い付きモーション
/// </summary>
class AirStickMotion {
public:

	// 更新
	void Update(float dt, ObjectComponent* object);
	// エアスティックデータ設定
	void SetData(const AirStickData& data) { 
		airStickData_ = data; 
		isPlaying_ = true;
	}
	// エアスティックデータ取得
	AirStickData GetData() const { return airStickData_; }

	// 再生中か取得
	bool IsPlaying() const { return isPlaying_; }

private:
	bool isPlaying_ = false;
	AirStickData airStickData_;	// エアスティックデータ
};


// カメラモーション
class CameraMotion {
public:

private:

};

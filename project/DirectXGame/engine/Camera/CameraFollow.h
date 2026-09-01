#pragma once
#include "DirectXGame/engine/Camera/CameraData.h"

// カメラ追従
/// <summary>
/// CameraFollowを管理・実装するクラス。
/// </summary>
class CameraFollow {
public:

	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update(Transform& transform,float dt);

	// ターゲット設定
	/// <summary>追従対象を設定する。</summary>
	/// <param name="target">追従対象への非所有ポインター。解除時はnullptr。</param>
	void SetTarget(const Engine::WorldTransform* target) { this->target = target; };

	/// <summary>
	/// カメラオフセット位置取得
	/// </summary>
	Vector3 GetOffsetPos() const { return data_.offset; }  
	/// <summary>
	/// カメラデフォルトオフセット位置取得
	/// </summary>
	Vector3 GetDefultOffsetPos() const { return defultOffst; }
	/// <summary>
	/// カメラオフセット位置設定
	/// </summary>
	void SetOffsetPos(const Vector3& offset) { data_.offset = offset; }
	/// <summary>
	/// 追従ターゲットに足すワールド座標オフセット設定
	/// </summary>
	void SetTargetOffset(const Vector3& offset) { data_.targetOffset = offset; }
	/// <summary>
	/// 追従ターゲットに足すワールド座標オフセット取得
	/// </summary>
	Vector3 GetTargetOffset() const { return data_.targetOffset; }
	/// <summary>
	/// カメラ位置取得
	/// </summary>
	Vector3 GetPos() const { return currentPos_; }
	/// <summary>
	/// データ取得
	/// </summary>
	CameraFollowData& GetData() { return data_; }
	const CameraFollowData& GetData() const { return data_; }
private:
	// データ
	CameraFollowData data_;
	// 現在のカメラ位置
	Vector3 currentPos_{};
	// デフォルトオフセット位置 
	Vector3 defultOffst = {};
	// 初回位置合わせ済みか
	bool isInitializedPosition_ = false;
	// ターゲット
	const Engine::WorldTransform* target = nullptr;
};

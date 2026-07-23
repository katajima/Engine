#pragma once
#include "DirectXGame/engine/Camera/CameraData.h"

namespace Engine {
	class Camera;
}

// カメラシェイク
/// <summary>
/// CameraShakeを管理・実装するクラス。
/// </summary>
class CameraShake {
public:

	/// <summary>
	/// リクエスト(時間と、揺れ)
	/// </summary>
	void Request(const CameraShakeData& data);

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Engine::Camera* camera);

	/// <summary>
	/// 更新
	/// </summary>
	void Update(Vector3& pos,float dt);

	/// <summary>
	/// シェイクしているか
	/// </summary>
	bool IsShake() const { return isShake; }

	/// <summary>
	/// カメラデフォルト位置設定
	/// </summary>
	void SetDefultPos(const Vector3& pos){defultPos = pos;};
	/// <summary>
	/// カメラデフォルト位置取得
	/// </summary>
	Vector3 GetDefultPos(){ return defultPos; }

private:
	// シェイクデータ
	CameraShakeData data;
	// シェイクしているか
	bool isShake = false;
	// カメラデフォルト位置
	Vector3 defultPos = {};
	// カメラ
	Engine::Camera* camera = nullptr;
};
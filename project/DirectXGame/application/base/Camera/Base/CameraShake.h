#pragma once
#include "DirectXGame/engine/Camera/CameraData.h"

namespace Engine {
	class Camera;
}

// カメラシェイク
class CameraShake {
public:

	// リクエスト(時間と、揺れ)
	void Request(const CameraShakeData& data);

	// 初期化
	void Initialize(Engine::Camera* camera);

	// 更新
	void Update(Vector3& pos,float dt);

	// シェイクしているか
	bool IsShake() const { return isShake; }

	// カメラデフォルト位置設定
	void SetDefultPos(const Vector3& pos){defultPos = pos;};
	// カメラデフォルト位置取得
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
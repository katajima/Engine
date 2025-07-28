#pragma once
#include"DirectXGame/application/base/BaseClass/Object/BaseObject.h"

// カメラの各速度
struct CameraSRTSpeed
{
	float scale = 0.0f;
	Vector3 rotate = {};
	float translate = 0.0f;
};


class CameraManeger;
class BaseCamera : public BaseObject
{
public:
	///< summary>
	/// 初期化
	///</summary>
	virtual void Initialize(Input* input, Entity3DManager* entity3DManager, Entity2DManager* entity2DManager, GlobalVariables* globalVariables, Vector3 position, Camera* camera) = 0;

	///< summary>
	/// 更新
	///</summary>
	virtual void Update() = 0;

	/// <summary>
	/// エフェクトの描画
	/// </summary>
	virtual void DrawEffect() = 0;


	virtual void Draw2D() =0;

	/// <summary>
	/// カメラ取得
	/// </summary>
	/// <returns></returns>
	Camera* GetUniqueCamera() { return uniqueCamera_.get(); }


	/// <summary>
	/// ロックオン対象を設定
	/// </summary>
	/// <param name="object"></param>
	void LockOn(BaseObject* object) { lockOnObject = object; };

	/// <summary>
	/// カメラ管理を設定
	/// </summary>
	/// <param name="cameraManeger"></param>
	void SetCameraManeger(CameraManeger* cameraManeger) { this->cameraManeger = cameraManeger; };
public:
	bool useCamera = false;

protected:
	std::unique_ptr<Camera> uniqueCamera_;	// 固有カメラ
	


	CameraSRTSpeed srtSpeed_;				// SRTスピード
	BaseObject* lockOnObject = nullptr;		// ロックオン対象
	CameraManeger* cameraManeger = nullptr;	// カメラ管理

};


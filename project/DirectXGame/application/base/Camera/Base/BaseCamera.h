#pragma once
#include "DirectXGame/application/base/Object/ObjectComponent.h"


// カメラの各速度
struct CameraSRTSpeed
{
	float scale = 0.0f;
	Vector3 rotate = {};
	float translate = 0.0f;
};


// 前方宣言
class CameraManeger;

/// <summary>
/// カメラ基底クラス
/// </summary>
class BaseCamera
{
public:
	virtual ~BaseCamera() = default; 
	///< summary>
	/// 初期化
	///</summary>
	virtual void Initialize(Input* input, Entity3DManager* entity3DManager,  GlobalVariables* globalVariables, Vector3 position) = 0;

	///< summary>
	/// 更新
	///</summary>
	virtual void Update() = 0;



	/// <summary>
	/// カメラ取得
	/// </summary>
	/// <returns></returns>
	Camera* GetUniqueCamera() { return uniqueCamera_.get(); }


	/// <summary>
	/// ロックオン対象を設定
	/// </summary>
	/// <param name="object"></param>
	void LockOn(Object3d* object) { lockOnObject = object; };

	/// <summary>
	/// カメラ管理を設定
	/// </summary>
	/// <param name="cameraManeger"></param>
	void SetCameraManeger(CameraManeger* cameraManeger) { this->cameraManeger = cameraManeger; };


	/// <summary>
	/// 時間取得
	/// </summary>
	/// <returns></returns>
	float GetTime() const;

	//名前設定
	void SetName(const std::string& name) { name_ = name; };
	// 名前取得
	std::string GetName() const { return name_; }

public:
	bool useCamera = false;

protected:
	std::unique_ptr<Camera> uniqueCamera_;	// 固有カメラ
	
	float timeSpeed_ = 1.0f;	// 時間の進む速さ(1.0fが通常、0.0fで停止、2.0fで2倍速など)
	std::string name_;

	CameraSRTSpeed srtSpeed_;				// SRTスピード
	Object3d* lockOnObject = nullptr;		// ロックオン対象
	CameraManeger* cameraManeger = nullptr;	// カメラ管理
protected:
	Entity3DManager* entity3DManager_ = nullptr;	// 3Dエンティティマネージャー
	GlobalVariables* globalVariables_ = nullptr;	// グローバル変数
	Input* input_ = nullptr;						// 入力(使わないならnullptr)

};

